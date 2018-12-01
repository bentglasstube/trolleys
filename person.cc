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

  if (pos == 0) {
    y_ = -8 - yjitter_dist(rand);
    speed_ = 0.02;
  } else {
    y_ = 248 + yjitter_dist(rand);
    speed_ = -0.02;
  }

  type_ = 0;
  timer_ = 0;
}

void Person::update(unsigned int elapsed) {
  timer_ += elapsed;
  y_ += elapsed * speed_;
}

void Person::draw(Graphics& graphics) const {
  const int frame = (timer_ / 100) % 3;
  sprites_.draw(graphics, 4 + frame, x_, y_);
}
