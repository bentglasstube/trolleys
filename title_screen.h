#pragma once

#include <vector>

#include "backdrop.h"
#include "graphics.h"
#include "input.h"
#include "screen.h"
#include "text.h"

#include "game_state.h"

class TitleScreen : public Screen {
  public:

    TitleScreen(GameState state);

    bool update(const Input& input, Audio& audio, unsigned int elapsed) override;
    void draw(Graphics& graphics) const override;

    Screen* next_screen() const override;
    std::string get_music_track() const override { return "dour.ogg"; }

  private:

    const std::vector<std::string> kChoices = {
      "Play Game",
      "Achievements",
      "Quit",
    };

    GameState gs_;
    Text text_;
    Backdrop backdrop_;

    int choice_;
};
