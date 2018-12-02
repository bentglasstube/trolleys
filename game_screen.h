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

    enum class State { Playing, Paused, Clear };

    struct Box {
      Box(int w, int h);

      int width, height;
      void draw(Graphics& graphics, const SpriteMap& ui) const;
    };

    Text text_;
    SpriteMap ui_;
    Map map_;

    State state_;

    int active_switch_ = 0;
    int train_timer_ = 5000;
    int person_timer_ = 500;
    int stage_timer_ = 120000;

    int deaths_ = 0;

    std::vector<Train> trains_;
    std::vector<Person> people_;
    std::vector<Particle> particles_;

    std::mt19937 rand_;

    void spawn_train(Audio& audio);
    void spawn_people(int count);

    void add_blood_spray(double x, double y, int n);
    void add_smoke(double x, double y, int n);

    std::string time_left_string() const;
};
