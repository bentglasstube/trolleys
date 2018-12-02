#include "person.h"

#include <random>

#include "util.h"

Person::Person() : sprites_("objects.png", 8, 16, 16) {
  std::mt19937 rand;
  rand.seed(Util::random_seed());

  std::uniform_int_distribution<int> pos_dist(0, 1);
  const int pos = pos_dist(rand);

  std::uniform_int_distribution<int> xjitter_dist(0, 8);
  std::uniform_int_distribution<int> yjitter_dist(0, 24);

  x_ = 204 + xjitter_dist(rand);

  std::uniform_real_distribution<double> speed_dist(0.007, 0.025);

  if (pos == 0) {
    y_ = -8 - yjitter_dist(rand);
    speed_ = speed_dist(rand);
  } else {
    y_ = 248 + yjitter_dist(rand);
    speed_ = -speed_dist(rand);
  }

  std::uniform_int_distribution<int> type_dist(0, 64);
  const int t = type_dist(rand);

  if (t < 48) type_ = 0;
  else if (t < 56) type_ = 1;
  else if (t < 60) type_ = 2;
  else type_ = 3;

  timer_ = 0;
  dead_ = false;
}

void Person::update(unsigned int elapsed) {
  if (dead_) return;
  timer_ += elapsed;
  y_ += elapsed * speed_;
}

void Person::kill() {
  dead_ = true;
}

void Person::draw(Graphics& graphics) const {
  if (dead_) return;
  const int frame = (timer_ / 100) % 3;
  sprites_.draw(graphics, 4 + frame + 8 * type_, x_, y_);
}

bool Person::collide(double x1, double x2, double y1, double y2) const {
  const double px1 = x_ + 5;
  const double px2 = px1 + 5;
  const double py1 = y_ + 9;
  const double py2 = py1 + 5;

  return x1 < px2 && x2 > px1 && y1 < py2 && y2 > py1;
}

bool Person::gone() const {
  return dead_ || y_ < -100 || y_ > 340;
}
