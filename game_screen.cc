#include "game_screen.h"

#include <algorithm>

GameScreen::GameScreen() : text_("text.png"), ui_("ui.png", 6, 16, 16), map_("level.txt") {
  rand_.seed(Util::random_seed());
}

bool GameScreen::update(const Input& input, Audio& audio, unsigned int elapsed) {
  // handle too big time steps
  if (elapsed > 25) elapsed = 25;

  const int count = map_.switch_count();
  if (input.key_pressed(Input::Button::Down)) {
    active_switch_ = (active_switch_ + 1) % count;
  } else if (input.key_pressed(Input::Button::Up)) {
    active_switch_ = (active_switch_ + count - 1) % count;
  }

  if (input.key_pressed(Input::Button::A)) {
    map_.toggle_switch(active_switch_);
  }

  train_timer_ -= elapsed;
  if (train_timer_ < 0) {
    spawn_train();
    std::uniform_int_distribution<int> train_dist(1500, 5000);
    train_timer_ += train_dist(rand_);
  }

  for (auto& p : people_) p.update(elapsed);

  for (auto& t : trains_) {
    t.update(map_, elapsed);

    for (auto& p : people_) {
      if (t.hit(p)) {
        p.kill();
        // TODO death animation, score, etc
      }
    }
  }

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

  for (const auto& p : people_) p.draw(graphics);
  for (const auto& t : trains_) {
    t.draw(graphics);

    if (t.x() < 0) {
      ui_.draw(graphics, 1, 0, t.y());
    }
  }
}

Screen* GameScreen::next_screen() const {
  return nullptr;
}

void GameScreen::spawn_train() {
  trains_.emplace_back(map_.random_track());
}

void GameScreen::spawn_person() {
  people_.emplace_back();
}
