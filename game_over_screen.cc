#include "game_over_screen.h"

#include "title_screen.h"

GameOverScreen::GameOverScreen(GameState state, int death_toll) : gs_(state), text_("text.png"), backdrop_("gameover.png"), death_toll_(death_toll), timer_(0), letters_(0), stage_(1) {}

bool GameOverScreen::update(const Input& input, Audio& audio, unsigned int elapsed) {
  if (!audio.music_playing()) audio.play_music("gameover.ogg");

  timer_ += elapsed;

  const std::string text = current_text();

  if (timer_ > kTextRate) {
    if (letters_ < text.length()) {
      ++letters_;
      if (text[letters_] != ' ') audio.play_sample("text.wav");
    }
    timer_ -= kTextRate;
  }

  if (letters_ >= text.length() && input.any_pressed()) {
    ++stage_;
    letters_ = 0;
    timer_ = 0;
  }

  if (text == "") {
    audio.stop_music();
    return false;
  }

  return true;
}

void GameOverScreen::draw(Graphics& graphics) const {
  backdrop_.draw(graphics);
  text_.draw(graphics, current_text().substr(0, letters_), 8, 144);
}

Screen* GameOverScreen::next_screen() const {
  return new TitleScreen(gs_);
}

std::string GameOverScreen::current_text() const {
  switch (stage_) {
    case 1:
      return "Over the course of five days,\n"
             "I have managed to kill " + std::to_string(death_toll_) + "\n"
             "living things.";
    case 2:
      return "If I am forced against my\n"
             "will into a situation where\n"
             "people will die, and I have no\n"
             "ability to stop it, how is my\n"
             "choice a 'moral' choice...\n";
    case 3:
      return "as opposed to a horror show\n"
             "I've just been thrust into,\n"
             "in which I have no\n"
             "meaningful agency?";
    case 4:
      return "That                       \n"
             "shit                       \n"
             "was                        \n"
             "terrible.";
    default:
      return "";
  }
}
