#pragma once

#include "graphics.h"
#include "spritemap.h"

class Person {
  public:

    Person();

    void update(unsigned int elapsed);
    void kill();

    void draw(Graphics& graphics) const;
    bool collide(double x1, double x2, double y1, double y2) const;
    bool gone() const;
    double x() const;
    double y() const;

  private:

    SpriteMap sprites_;

    double x_, y_, speed_;
    int type_, timer_;
    bool dead_;
};
