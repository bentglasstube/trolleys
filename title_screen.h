#pragma once

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

  private:

    GameState gs_;
    Text text_;
    Backdrop backdrop_;
};
