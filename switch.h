#pragma once

#include "graphics.h"
#include "spritemap.h"

class Switch {
  public:

    enum class Direction { Upward, Downward };
    enum class State { Straight, Diversion };

    Switch(int x, int y, Direction dir);

    bool divert() const;
    bool at(int x, int y) const;
    Direction dir() const;
    int x() const;
    int y() const;

    void toggle();
    void draw(Graphics& graphics) const;

  private:

    SpriteMap sprites_;
    Direction dir_;
    State state_;
    int x_, y_;

};
