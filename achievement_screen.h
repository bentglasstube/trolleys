#pragma once

#include "backdrop.h"
#include "input.h"
#include "screen.h"
#include "text.h"

#include "game_state.h"

class AchievementScreen : public Screen {
  public:

    AchievementScreen(GameState state);

    bool update(const Input& input, Audio& audio, unsigned int elapsed) override;
    void draw(Graphics& graphics) const override;

    Screen* next_screen() const override;
    std::string get_music_track() const override { return "dour.ogg"; }

  private:

    static constexpr int kAchievements = 6;

    GameState gs_;
    Text text_;
    Backdrop backdrop_;
    int select_;

    void draw_achievement(Graphics& graphics, GameState::Achievement a, int x, int y) const;
};
