#pragma once

#include <random>
#include <vector>

#include "graphics.h"
#include "input.h"
#include "particle.h"
#include "screen.h"
#include "spritemap.h"
#include "text.h"
#include "util.h"

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
    SpriteMap ui_;
    Map map_;

    int active_switch_ = 0;
    int train_timer_ = 5000;
    int person_timer_ = 500;

    std::vector<Train> trains_;
    std::vector<Person> people_;
    std::vector<Particle> particles_;

    std::mt19937 rand_;

    void spawn_train();
    void spawn_people(int count);

    void add_blood_spray(double x, double y, int n);
    void add_smoke(double x, double y, int n);
};

