#include "game_screen.h"

GameScreen::GameScreen() : text_("text.png"), tracks_("tracks.png", 6, 16, 16), map_("level.txt") {}

bool GameScreen::update(const Input& input, Audio& audio, unsigned int elapsed) {
  const int count = map_.switch_count();
  if (input.key_pressed(Input::Button::Down)) {
    active_switch_ = (active_switch_ + 1) % count;
  } else if (input.key_pressed(Input::Button::Up)) {
    active_switch_ = (active_switch_ + count - 1) % count;
  }

  if (input.key_pressed(Input::Button::A)) {
    map_.toggle_switch(active_switch_);
  }

  if (input.key_pressed(Input::Button::B)) {
    spawn_train();
  }

  for (auto& t : trains_) {
    t.update(map_, elapsed);
    // TODO remove defunct trains
  }

  return true;
}

void GameScreen::draw(Graphics& graphics) const {
  map_.draw(graphics);

  const auto& active = map_.get_switch(active_switch_);
  tracks_.draw(graphics, 5, active.x(), active.y());

  for (const auto& t : trains_) {
    t.draw(graphics);
  }
}

Screen* GameScreen::next_screen() const {
  return nullptr;
}

void GameScreen::spawn_train() {
  trains_.emplace_back(map_.random_track());
}
