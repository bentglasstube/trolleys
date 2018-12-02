#pragma once

#include <vector>

#include "graphics.h"
#include "spritemap.h"

#include "switch.h"

class Map {
  public:

    struct SwitchResult {
      enum class Direction { None, DivertUp, DivertDown };

      Direction dir;
      int x;
    };


    Map(const std::string& file);

    void load(const std::string& file);
    void toggle_switch(size_t  n);

    void draw(Graphics& graphics) const;
    size_t switch_count() const;
    const Switch& get_switch(size_t n) const;
    int random_track() const;

    SwitchResult check_switch(int x1, int x2, int y) const;
    int tile_at(int x, int y) const;

  private:

    static constexpr int kHeight = 15;
    static constexpr int kWidth = 16;

    SpriteMap tracks_;
    int tiles_[kHeight][kWidth];
    std::vector<Switch> switches_;

};
