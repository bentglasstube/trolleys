#include "switch.h"

Switch::Switch(int x, int y, Direction dir) :
  sprites_("tracks.png", 6, 16, 16),
  dir_(dir),
  state_(State::Straight),
  x_(x), y_(y) {}

bool Switch::divert() const {
  return state_ == State::Diversion;
}

bool Switch::at(int x, int y) const {
  return x == x_ && y == y_;
}

Switch::Direction Switch::dir() const {
  return dir_;
}

int Switch::x() const {
  return x_;
}

int Switch::y() const {
  return y_;
}

void Switch::toggle() {
  state_ = state_ == State::Straight ? State::Diversion : State::Straight;
}

void Switch::draw(Graphics& graphics) const {
  const int n = (dir_ == Direction::Upward ? 19 : 1) + (divert() ? 2 : 0);
  sprites_.draw(graphics, n, x_, y_);
  sprites_.draw(graphics, n + 1, x_ + 16, y_);
}
