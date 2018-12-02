#include "map.h"

#include <fstream>
#include <iostream>
#include <random>
#include <sstream>

#include "util.h"

Map::Map(const std::string& file) : tracks_("tracks.png", 6, 16, 16) {
  load(file);
}

void Map::load(const std::string& file) {
  switches_.clear();

  std::ifstream f("content/" + file);
  if (f.is_open()) {
    std::string line;
    int y = 0;
    while (getline(f, line)) {
      // TODO check length of line
      for (int x = 0; x < kWidth; ++x) {
        const int value = line[x] - 'A';
        tiles_[y][x] = (value < 0 || value > 23) ? 0: value;

        if (value == 1) {
          switches_.emplace_back(x * 16, y * 16, Switch::Direction::Downward);
        } else if (value == 19) {
          switches_.emplace_back(x * 16, y * 16, Switch::Direction::Upward);
        }
      }
      ++y;
    }
    f.close();
  }
}

void Map::toggle_switch(size_t n) {
  if (n < 0) return;
  if (n >= switches_.size()) return;
  switches_[n].toggle();
}

void Map::draw(Graphics& graphics) const {
  for (int y = 0; y < kHeight; ++y) {
    for (int x = 0; x < kWidth; ++x) {
      tracks_.draw(graphics, tiles_[y][x], x * 16, y * 16);
    }
  }

  for (const auto& s : switches_) s.draw(graphics);
}

size_t Map::switch_count() const {
  return switches_.size();
}

const Switch& Map::get_switch(size_t n) const {
  return switches_[n];
}

int Map::random_track() const {
  std::mt19937 rand;
  rand.seed(Util::random_seed());

  int t = 0;
  int count = 0;
  for (int y = 0; y < kHeight; ++y) {
    if (tiles_[y][0] != 0) {
      std::uniform_int_distribution<int> choose(1, ++count);
      if (choose(rand) == 1) t = y;
    }
  }

  return t * 16;
}

Map::SwitchResult Map::check_switch(int x1, int x2, int y) const {
  for (int x = 0; x < kWidth; ++x) {
    const int sx = 16 * x + 4;
    if (x1 < sx && x2 >= sx) {
      for (const auto& s : switches_) {
        if (s.at(x * 16, y) && s.divert()) {
          const SwitchResult::Direction d = s.dir() == Switch::Direction::Upward ? SwitchResult::Direction::DivertUp : SwitchResult::Direction::DivertDown;
          return { d, sx };
        }
      }
    }
  }

  return { SwitchResult::Direction::None, 0 };
}

int Map::tile_at(int x, int y) const {
  return  tiles_[y / 16][x / 16];
}
