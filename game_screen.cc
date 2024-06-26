#include "game_screen.h"

#include <algorithm>
#include <cmath>

#include "game_over_screen.h"

GameScreen::GameScreen(GameState state) : gs_(state), text_("text.png"), ui_("ui.png", 3, 16, 16), map_("level1.txt"), state_(State::Playing) {
  rand_.seed(Util::random_seed());
  reload();
}

bool GameScreen::update(const Input& input, Audio& audio, unsigned int elapsed) {
  // handle too big time steps
  if (elapsed > 25) elapsed = 25;

  // TODO check for achievements
  const GameState::Achievement a = gs_.alert_needed();
  if (a != GameState::Achievement::Nothing) {
    alerts_.emplace_back(GameState::Name(a));
    gs_.show(a);
    audio.play_sample("achieve.wav");
    last_achieve_ = a;
  }

  if (!alerts_.empty()) {
    alerts_[0].update(elapsed);
    if (alerts_[0].timer > 3500) alerts_.erase(alerts_.begin());
  }

  if (state_ == State::Playing) {
    raininess_ += elapsed;
    stage_timer_ -= elapsed;

    if (stage_timer_ < 0) stage_timer_ = 0;
    add_raindrop(raininess_ / 500);
    if (raininess_ > 100000) {
      lightning_timer_ -= elapsed;
      if (lightning_timer_ < 0) {
        std::uniform_int_distribution<int> lit_dist(8000, 12000);
        lightning_timer_ = lit_dist(rand_);
        audio.play_sample("thunder.wav");
      }
    }

#ifndef NDEBUG
    if (input.key_pressed(Input::Button::Select)) {
      stage_timer_ -= 10000;
      raininess_ += 10000;
    }

    if (input.key_pressed(Input::Button::B)) {
      alerts_.emplace_back("Test Achievement: " + std::to_string(current_deaths_));
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
          t.taint();
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
    if (current_deaths_ == 0) gs_.achieve(GameState::Achievement::Deathless);
    if (dog_killed_ == false) gs_.achieve(GameState::Achievement::Dogless);

    if (input.key_pressed(Input::Button::Start)) {
      ++level_;

      if (level_ > kMaxLevel) return false;

      reload();
    }
  }

  return true;
}

void GameScreen::draw(Graphics& graphics) const {
  map_.draw(graphics);

  for (const auto& p : people_) p.draw(graphics);
  for (const auto& t : trains_) {
    t.draw(graphics);
    if (t.x() < 0) ui_.draw(graphics, 1, 0, t.y());
  }

  for (const auto& p : particles_) p.draw(graphics);

  SDL_Rect r = { 0, 0, 256, 240 };
  const int gloom = 25 * (level_ - 1);
  graphics.draw_rect(&r, 0x00000000 | gloom, true);

  if (lightning_timer_ > 0 && lightning_timer_ < 510) {
    const int brightness = 250 - std::abs(250 - lightning_timer_);
    graphics.draw_rect(&r, 0xffffff00 | brightness, true);
  }

  const auto& active = map_.get_switch(active_switch_);
  ui_.draw(graphics, 0, active.x(), active.y());

  ui_.draw(graphics, 2, 0, 0);
  text_.draw(graphics, std::to_string(current_deaths_), 16, 0);
  text_.draw(graphics, time_left_string(), 256, 0, Text::Alignment::Right);
  text_.draw(graphics, "Day " + std::to_string(level_), 128, 0, Text::Alignment::Center);

  if (state_ == State::Paused) {

    const Box b(10, 3);
    b.draw(graphics, ui_);
    text_.draw(graphics, "P A U S E D", 128, 112, Text::Alignment::Center);

    if (last_achieve_ != GameState::Achievement::Nothing) {
      text_.draw(graphics, GameState::Name(last_achieve_), 128, 184, Text::Alignment::Center);
      text_.draw(graphics, GameState::Description(last_achieve_), 32, 208);
    }

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

  if (!alerts_.empty()) alerts_[0].draw(graphics, ui_, text_);
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

void GameScreen::add_raindrop(int n) {
  std::uniform_int_distribution<int> xd(0, 288);
  std::uniform_int_distribution<int> yd(-32, 240);

  for (int i = 0; i < n; ++i) {
    particles_.emplace_back(xd(rand_), yd(rand_), -0.03, 0.3, 0, 0x8888ffff, 500);
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

GameScreen::Achievement::Achievement(const std::string& text) : text(text), width(8 * text.length() + 32), timer(0) {}

void GameScreen::Achievement::update(unsigned int elapsed) {
  timer += elapsed;
}

void GameScreen::Achievement::draw(Graphics& graphics, const SpriteMap& ui, const Text& t) const {
  const int sx = 128 - width / 2;

  int y = 0;
  if (timer < 500) {
    y = 240 - (32 * timer / 500);
  } else if (timer < 2500) {
    y = 208;
  } else {
    y = 240 - (32 * (3000 - timer) / 500);
  }

  ui.draw(graphics, 12, sx, y);
  ui.draw(graphics, 15, sx, y + 16);

  for (int x = sx + 16; x < sx + width - 16; x += 16) {
    ui.draw(graphics, 13, x, y);
    ui.draw(graphics, 16, x, y + 16);
  }

  ui.draw(graphics, 14, sx + width - 16, y);
  ui.draw(graphics, 17, sx + width - 16, y + 16);

  t.draw(graphics, text, sx + 24, y + 8);
}
