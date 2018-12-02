#include "train.h"

#include <random>

#include "util.h"

Train::Train(int y) : sprites_("objects.png", 8, 16, 16) {
  std::mt19937 rand;
  rand.seed(Util::random_seed());

  std::uniform_int_distribution<int> car_count(1, kMaxCars);
  const int cars = car_count(rand);

  std::uniform_int_distribution<int> car_type(1, 2);
  cars_.emplace_back(-kLeftOffset, y, 0);
  for (int i = 1; i < cars; ++i) {
    cars_.emplace_back(-kLeftOffset - i * 16, y, car_type(rand));
  }

  timer_ = 0;
  // TODO randomize speed
  speed_ = 0.1;
}

void Train::update(const Map& map, unsigned int elapsed) {
  timer_ += elapsed;
  for (auto& c : cars_) {
    c.update(map, elapsed, speed_);
  }
}

void Train::draw(Graphics& graphics) const {
  const int frame = (timer_ / 250) % 3;

  for (const auto& c : cars_) {
    const int n = 8 * c.type + frame;
    sprites_.draw(graphics, n, c.x - 16, c.y);

#ifndef NDEBUG
    if (c.dir != Train::Direction::Forward) {
      auto tc = c.tile_coords();
      SDL_Rect r = { tc.first, tc.second, 16, 16 };
      graphics.draw_rect(&r, 0xd8ff00ff, false);
    }
#endif
  }
}

bool Train::hit(const Person& person) const {
  for (const auto& c : cars_) {
    if (c.hit(person)) return true;
  }
  return false;
}

bool Train::gone() const {
  return cars_[0].x > 320 + 16 * kMaxCars;
}

double Train::x() const {
  return cars_[0].x;
}

double Train::y() const {
  return cars_[0].y;
}

Train::TrainCar::TrainCar(double x, double y, int type) : x(x), y(y), dir(Direction::Forward), type(type) {}

void Train::TrainCar::update(const Map& map, unsigned int elapsed, double speed) {
  const double nx = x + speed * elapsed;

  auto r = map.check_switch(x, nx, y);
  if (r.dir == Map::SwitchResult::Direction::DivertUp) {
    dir = Train::Direction::Upward;
    divx = r.x;
    divy = y;
  } else if (r.dir == Map::SwitchResult::Direction::DivertDown) {
    dir = Train::Direction::Downward;
    divx = r.x;
    divy = y;
  }

  if (dir != Train::Direction::Forward) {
    const double dy = diversion_offset();
    const double ny = (dir == Train::Direction::Upward) ? divy - dy : divy + dy;

    const auto tc = tile_coords();
    if (map.check_joiner(tc.first, tc.second)) {
      dir = Train::Direction::Forward;
      y = (int)((ny + 8) / 16) * 16;
    } else {
      y = ny;
    }
  }

  x = nx;
}

double Train::TrainCar::diversion_offset() const {
  const int dx = x - divx;
  if (dx < 10) return 0;
  if (dx < 13) return 1;
  if (dx < 17) return 2;
  if (dx < 20) return 3;
  return (dx - 20) / 2.0 + 3;
}

bool Train::TrainCar::hit(const Person& person) const {
  return person.collide(x - 14, x - 2, y + 5, y + 11);
}

std::pair<int, int> Train::TrainCar::tile_coords() const {
  const int tx = (int)((x - 16) / 16) * 16;
  const int ty = (int)(y / 16) * 16;
  return std::move(std::make_pair(tx, ty));
}
