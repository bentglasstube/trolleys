#pragma once

#include "backdrop.h"
#include "screen.h"
#include "text.h"

class GameOverScreen : public Screen {
  public:

    GameOverScreen(int death_toll);

    bool update(const Input& input, Audio& audio, unsigned int elapsed) override;
    void draw(Graphics& graphics) const override;

    Screen* next_screen() const override;

  private:

    static constexpr int kTextRate = 25;

    Text text_;
    Backdrop backdrop_;

    int death_toll_;
    unsigned long timer_, letters_, stage_;

    std::string current_text() const;

};
