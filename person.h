#pragma once

#include "graphics.h"
#include "spritemap.h"

#include "train.h"

class Person {
  public:

    Person();

    void update(unsigned int elapsed);
    void draw(Graphics& graphics) const;

    bool hit(const Train& train) const;

  private:

    SpriteMap sprites_;

    double x_, y_, speed_;
    int type_, timer_;
};
