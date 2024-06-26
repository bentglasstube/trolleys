#pragma once

#include <random>
#include <vector>

#include "particle.h"
#include "screen.h"
#include "spritemap.h"
#include "text.h"
#include "util.h"

#include "game_state.h"
#include "map.h"
#include "person.h"
#include "train.h"

class GameScreen : public Screen {
  public:

    GameScreen(GameState state);

    bool update(const Input& input, Audio& audio, unsigned int elapsed) override;
    void draw(Graphics& graphics) const override;

    Screen* next_screen() const override;
    std::string get_music_track() const override { return "dour.ogg"; }

  private:

    static constexpr int kMaxLevel = 5;

    enum class State { Playing, Paused, Clear };

    struct Box {
      Box(int w, int h);

      int width, height;
      void draw(Graphics& graphics, const SpriteMap& ui) const;
    };

    struct Achievement {
      Achievement(const std::string& text);
      std::string text;
      int width, timer;
      void update(unsigned int elapsed);
      void draw(Graphics& graphics, const SpriteMap& ui, const Text& t) const;
    };

    GameState gs_;
    Text text_;
    SpriteMap ui_;
    Map map_;

    State state_;

    int active_switch_ = 0;
    int train_timer_ = 5000;
    int person_timer_ = 500;
    int stage_timer_ = 120000;

    int current_deaths_ = 0;
    int total_deaths_ = 0;

    bool dog_killed_ = false;
    int raininess_ = 0;
    int lightning_timer_ = 0;

    int level_ = 1;

    GameState::Achievement last_achieve_ = GameState::Achievement::Nothing;

    std::vector<Train> trains_;
    std::vector<Person> people_;
    std::vector<Particle> particles_;
    std::vector<Achievement> alerts_;

    std::mt19937 rand_;

    void reload();

    void spawn_train(Audio& audio);
    void spawn_people(int count);

    void add_blood_spray(double x, double y, int n);
    void add_smoke(double x, double y, int n);
    void add_raindrop(int n);

    std::string time_left_string() const;
};
