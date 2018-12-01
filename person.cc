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
}

void Person::update(unsigned int elapsed) {
  timer_ += elapsed;
  y_ += elapsed * speed_;
}

void Person::draw(Graphics& graphics) const {
  const int frame = (timer_ / 100) % 3;
  sprites_.draw(graphics, 4 + frame + 8 * type_, x_, y_);
}
