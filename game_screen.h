#pragma once

#include <vector>

#include "graphics.h"
#include "input.h"
#include "screen.h"
#include "spritemap.h"
#include "text.h"

#include "map.h"
#include "train.h"
#include "person.h"

class GameScreen : public Screen {
  public:

    GameScreen();

    bool update(const Input& input, Audio& audio, unsigned int elapsed) override;
    void draw(Graphics& graphics) const override;

    Screen* next_screen() const override;

  private:

    Text text_;
    SpriteMap tracks_;
    Map map_;

    int active_switch_ = 0;

    std::vector<Train> trains_;
    std::vector<Person> people_;

    void spawn_train();
    void spawn_person();
};

