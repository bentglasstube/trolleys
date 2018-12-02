#include "title_screen.h"

#include "game_screen.h"

TitleScreen::TitleScreen() : text_("text.png"), backdrop_("title.png") {}

bool TitleScreen::update(const Input& input, Audio& audio, unsigned int) {
  return !input.any_pressed();
}

void TitleScreen::draw(Graphics& graphics) const {
  backdrop_.draw(graphics);
  text_.draw(graphics, "Press Any Key", 128, 208, Text::Alignment::Center);
}

Screen* TitleScreen::next_screen() const {
  return new GameScreen();
}
