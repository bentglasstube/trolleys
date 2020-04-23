#include "title_screen.h"

#include "achievement_screen.h"
#include "game_screen.h"

TitleScreen::TitleScreen(GameState state) : gs_(state), text_("text.png"), backdrop_("title.png"), choice_(0) {}

bool TitleScreen::update(const Input& input, Audio& audio, unsigned int) {
  if (input.key_pressed(Input::Button::Up)) {
    --choice_;
    audio.play_sample("select.wav");
  }
  if (input.key_pressed(Input::Button::Down)) {
    ++choice_;
    audio.play_sample("select.wav");
  }

  choice_ = (choice_ + kChoices.size()) % kChoices.size();

  if (input.key_pressed(Input::Button::A) || input.key_pressed(Input::Button::Start)) {
    audio.play_sample("change.wav");
    return false;
  }

  return true;
}

void TitleScreen::draw(Graphics& graphics) const {
  backdrop_.draw(graphics);

  for (size_t i = 0; i < kChoices.size(); ++i) {
    text_.draw(graphics, kChoices[i], 144, 168 + 16 * i);
  }
  text_.draw(graphics, ">", 128, 168 + 16 * choice_);
}

Screen* TitleScreen::next_screen() const {
  switch (choice_) {
    case 0: return new GameScreen(gs_);
    case 1: return new AchievementScreen(gs_);
    default: return nullptr;
  }
}
