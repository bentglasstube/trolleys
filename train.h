#pragma once

#include <vector>

#include "graphics.h"
#include "spritemap.h"

#include "map.h"

class Train {
  public:

    Train(int y);

    void update(const Map& map, unsigned int elapsed);

    void draw(Graphics& graphics) const;

  private:

    static constexpr int kMaxCars = 4;

    enum class Direction { Forward, Upward, Downward };

    struct TrainCar {
      double x, y;
      double divx, divy;
      Direction dir;
      int type;

      TrainCar(double x, double y, int type);

      void update(const Map& map, unsigned int elapsed, double speed);
      double diversion_offset() const;

      std::pair<int, int> tile_coords() const;
    };

    SpriteMap sprites_;

    std::vector<TrainCar> cars_;
    int timer_;
    double speed_;

};

