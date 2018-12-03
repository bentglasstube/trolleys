#include "achievement_screen.h"

#include "title_screen.h"

AchievementScreen::AchievementScreen(GameState state) : gs_(state), text_("text.png"), backdrop_("achievements.png"), select_(0) {}

bool AchievementScreen::update(const Input& input, Audio& audio, unsigned int) {
  if (input.key_pressed(Input::Button::Up)) {
    audio.play_sample("select.wav");
    --select_;
  }
  if (input.key_pressed(Input::Button::Down)) {
    audio.play_sample("select.wav");
    ++select_;
  }

  select_ = (select_ + kAchievements) % kAchievements;

  if (input.key_pressed(Input::Button::A)) return false;
  if (input.key_pressed(Input::Button::B)) return false;
  if (input.key_pressed(Input::Button::Start)) return false;

  return true;
}

void AchievementScreen::draw(Graphics& graphics) const {
  backdrop_.draw(graphics);

  draw_achievement(graphics, GameState::Achievement::Missed,    48, 32);
  draw_achievement(graphics, GameState::Achievement::Deathless, 48, 48);
  draw_achievement(graphics, GameState::Achievement::Dogless,   48, 64);
  draw_achievement(graphics, GameState::Achievement::Split,     48, 80);
  draw_achievement(graphics, GameState::Achievement::Rejoin,    48, 96);
  draw_achievement(graphics, GameState::Achievement::ThreeWay,  48, 112);

  text_.draw(graphics, ">", 32, 32 + select_ * 16);

  const GameState::Achievement a = static_cast<GameState::Achievement>(select_ + 1);
  if (gs_.known(a)) {
    text_.draw(graphics, GameState::Description(a), 32, 144);
  }
}

Screen* AchievementScreen::next_screen() const {
  return new TitleScreen(gs_);
}

void AchievementScreen::draw_achievement(Graphics& graphics, GameState::Achievement a, int x, int y) const {
  std::string t = gs_.known(a) ? GameState::Name(a) : "???";
  text_.draw(graphics, t, x, y);
}
