#include "game_screen.h"

#include <algorithm>
#include <cmath>

#include "game_over_screen.h"

GameScreen::GameScreen(GameState state) : gs_(state), text_("text.png"), ui_("ui.png", 3, 16, 16), map_("level1.txt"), state_(State::Playing) {
  rand_.seed(Util::random_seed());
  reload();
}

bool GameScreen::update(const Input& input, Audio& audio, unsigned int elapsed) {
  if (!audio.music_playing()) audio.play_music("dour.ogg");

  // handle too big time steps
  if (elapsed > 25) elapsed = 25;

  if (state_ == State::Playing) {
    stage_timer_ -= elapsed;
    if (stage_timer_ < 0) stage_timer_ = 0;

#ifndef NDEBUG
    if (input.key_pressed(Input::Button::Select)) {
      stage_timer_ -= 10000;
    }
#endif

    if (input.key_pressed(Input::Button::Start)) {
      state_ = State::Paused;
      audio.play_sample("pause.wav");
      return true;
    }

    const int count = map_.switch_count();
    if (input.key_pressed(Input::Button::Down)) {
      active_switch_ = (active_switch_ + 1) % count;
      audio.play_sample("select.wav");
    } else if (input.key_pressed(Input::Button::Up)) {
      active_switch_ = (active_switch_ + count - 1) % count;
      audio.play_sample("select.wav");
    }

    if (input.key_pressed(Input::Button::A)) {
      map_.toggle_switch(active_switch_);
      audio.play_sample("change.wav");
    }

    if (stage_timer_ > 0) {
      train_timer_ -= elapsed;
      if (train_timer_ < 0) {
        spawn_train(audio);
        std::uniform_int_distribution<int> train_dist(1500, 5000);
        train_timer_ += train_dist(rand_);
      }

      person_timer_ -= elapsed;
      if (person_timer_ < 0) {
        std::uniform_int_distribution<int> people_count_dist(1, 5);
        spawn_people(people_count_dist(rand_));

        std::uniform_int_distribution<int> people_timer_dist(3000, 6000);
        person_timer_ += people_timer_dist(rand_);
      }

    }

    for (auto& p : particles_) p.update(elapsed);
    for (auto& p : people_) p.update(elapsed);

    for (auto& t : trains_) {
      t.update(map_, elapsed, gs_);

      add_smoke(t.x() - 6.5, t.y() + 2, 10);

      for (auto& p : people_) {
        if (t.hit(p)) {
          p.kill();
          add_blood_spray(p.x() + 7, p.y() + 8, 200);
          audio.play_sample("hit.wav");
          ++current_deaths_;

          if (p.is_dog()) dog_killed_ = true;
        }
      }
    }

    particles_.erase(std::remove_if(
          particles_.begin(), particles_.end(),
          [](const Particle& p){ return p.done(); }),
        particles_.end());

    people_.erase(std::remove_if(
          people_.begin(), people_.end(),
          [](const Person& p){ return p.gone(); }),
        people_.end());

    trains_.erase(std::remove_if(
          trains_.begin(), trains_.end(),
          [](const Train& t){ return t.gone(); }),
        trains_.end());

    if (stage_timer_ == 0  && trains_.empty()) {
      total_deaths_ += current_deaths_;
      state_ = State::Clear;
    }

  } else if (state_ == State::Paused) {
    if (input.key_pressed(Input::Button::Start)) {
      state_ = State::Playing;
      audio.play_sample("pause.wav");
    }
  } else if (state_ == State::Clear) {
    if (input.key_pressed(Input::Button::Start)) {
      ++level_;

      if (current_deaths_ == 0) gs_.deathless |= 1;
      if (dog_killed_ == false) gs_.dogless |= 1;

      if (level_ > kMaxLevel) {
        audio.stop_music();
        return false;
      }

      reload();
    }
  }

  return true;
}

void GameScreen::draw(Graphics& graphics) const {
  map_.draw(graphics);

  // TODO move selector to ui spritemap
  const auto& active = map_.get_switch(active_switch_);
  ui_.draw(graphics, 0, active.x(), active.y());

  ui_.draw(graphics, 2, 0, 0);
  text_.draw(graphics, std::to_string(current_deaths_), 16, 0);
  text_.draw(graphics, time_left_string(), 256, 0, Text::Alignment::Right);
  text_.draw(graphics, "Day " + std::to_string(level_), 128, 224, Text::Alignment::Center);

  for (const auto& p : people_) p.draw(graphics);
  for (const auto& t : trains_) {
    t.draw(graphics);
    if (t.x() < 0) ui_.draw(graphics, 1, 0, t.y());
  }

  for (const auto& p : particles_) p.draw(graphics);

  if (state_ == State::Paused) {

    const Box b(10, 3);
    b.draw(graphics, ui_);
    text_.draw(graphics, "P A U S E D", 128, 112, Text::Alignment::Center);

  } else if (state_ == State::Clear) {

    const Box b(12, 7);
    b.draw(graphics, ui_);
    text_.draw(graphics, "Stage Clear", 128, 72, Text::Alignment::Center);

    text_.draw(graphics, "Killed Today: ", 48, 104);
    text_.draw(graphics, std::to_string(current_deaths_), 208, 104, Text::Alignment::Right);

    text_.draw(graphics, "Total Deaths: ", 48, 120);
    text_.draw(graphics, std::to_string(total_deaths_), 208, 120, Text::Alignment::Right);

    text_.draw(graphics, "Press Start", 128, 152, Text::Alignment::Center);

  }
}

Screen* GameScreen::next_screen() const {
  return new GameOverScreen(gs_, total_deaths_);
}

void GameScreen::reload() {
  map_.load("level" + std::to_string(level_) + ".txt");

  trains_.clear();
  people_.clear();
  particles_.clear();

  active_switch_ = 0;
  train_timer_ = 5000;
  person_timer_ = 500;
  stage_timer_ = level_ * 30000;

  state_ = State::Playing;
  current_deaths_ = 0;
  dog_killed_ = false;
}

void GameScreen::spawn_train(Audio& audio) {
  trains_.emplace_back(map_.random_track());
  audio.play_sample("alert.wav");
}

void GameScreen::spawn_people(int count) {
  for (int i = 0; i < count; ++i) people_.emplace_back();
}

void GameScreen::add_blood_spray(double x, double y, int n) {
  std::uniform_real_distribution<float> angle_dist(0.1, 1.5);
  std::uniform_real_distribution<float> vel_dist(0.02, 0.3);
  std::uniform_int_distribution<int> red_dist(0x80, 0xff);
  for (int i = 0; i < n; ++i) {
    const float a = angle_dist(rand_);
    const float v = vel_dist(rand_);
    const int r = red_dist(rand_);
    particles_.emplace_back(x, y, v * std::cos(a), v * -std::sin(a), 0.001f, r << 24 | 0xff, 400);
  }
}

void GameScreen::add_smoke(double x, double y, int n) {
  std::uniform_real_distribution<float> pos_dist(-1.5, 1.5);
  std::uniform_real_distribution<float> vel_dist(0.01, 0.02);
  std::uniform_int_distribution<int> color_dist(0x50, 0xd0);
  for (int i = 0; i < n; ++i) {
    const int c = color_dist(rand_);
    const unsigned long color = c << 24 | c << 16 | c << 8 | 0xff;
    particles_.emplace_back(x + pos_dist(rand_), y + pos_dist(rand_), 0, -vel_dist(rand_), 0.0f, color, 800);
  }
}

std::string GameScreen::time_left_string() const {
  const int m = stage_timer_ / 60000;
  const int s = (stage_timer_ / 1000) % 60;

  return std::to_string(m) + ":" + (s < 10 ? "0" : "") + std::to_string(s);
}

GameScreen::Box::Box(int w, int h) : width(w), height(h) {}

void GameScreen::Box::draw(Graphics& graphics, const SpriteMap& ui) const {
  const int sx = 128 - width * 8;
  const int sy = 120 - height * 8;
  const int ex = 112 + width * 8;
  const int ey = 104 + height * 8;

  for (int y = 1; y < height - 1; ++y) {
    ui.draw(graphics, 6, sx, sy + 16 * y);
    ui.draw(graphics, 8, ex, sy + 16 * y);
  }

  for (int x = 1; x < width - 1; ++x) {
    ui.draw(graphics, 4, sx + 16 * x, sy);
    for (int y = 1; y < height - 1; ++y) ui.draw(graphics, 7, sx + 16 * x, sy + 16 * y);
    ui.draw(graphics, 10, sx + 16 * x, ey);
  }

  ui.draw(graphics, 3, sx, sy);
  ui.draw(graphics, 5, ex, sy);
  ui.draw(graphics, 9, sx, ey);
  ui.draw(graphics, 11, ex, ey);
}
