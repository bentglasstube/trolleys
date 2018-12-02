#include "game_screen.h"

#include <algorithm>
#include <cmath>

GameScreen::GameScreen() : text_("text.png"), ui_("ui.png", 6, 16, 16), map_("level.txt") {
  rand_.seed(Util::random_seed());
}

bool GameScreen::update(const Input& input, Audio& audio, unsigned int elapsed) {
  // handle too big time steps
  if (elapsed > 25) elapsed = 25;

  stage_timer_ -= elapsed;
  if (stage_timer_ < 0) stage_timer_ = 0;

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

    if (people_.empty() && trains_.empty()) {
      // TODO level complete
    }
  }

  for (auto& p : particles_) p.update(elapsed);
  for (auto& p : people_) p.update(elapsed);

  for (auto& t : trains_) {
    t.update(map_, elapsed);

    add_smoke(t.x() - 6.5, t.y() + 2, 10);

    for (auto& p : people_) {
      if (t.hit(p)) {
        p.kill();
        add_blood_spray(p.x() + 7, p.y() + 8, 200);
        audio.play_sample("hit.wav");
        ++deaths_;
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

  return true;
}

void GameScreen::draw(Graphics& graphics) const {
  map_.draw(graphics);

  // TODO move selector to ui spritemap
  const auto& active = map_.get_switch(active_switch_);
  ui_.draw(graphics, 0, active.x(), active.y());

  ui_.draw(graphics, 2, 0, 0);
  text_.draw(graphics, std::to_string(deaths_), 16, 0);

  text_.draw(graphics, time_left_string(), 256, 0, Text::Alignment::Right);

  for (const auto& p : people_) p.draw(graphics);
  for (const auto& t : trains_) {
    t.draw(graphics);

    if (t.x() < 0) {
      ui_.draw(graphics, 1, 0, t.y());
    }
  }

  for (const auto& p : particles_) p.draw(graphics);
}

Screen* GameScreen::next_screen() const {
  return nullptr;
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

