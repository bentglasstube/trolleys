#include "game_state.h"

#include <string>

GameState::GameState() {}

void GameState::achieve(Achievement a) {
  status_.emplace(a, Status::Gotten);
}

void GameState::show(Achievement a) {
  if (status_[a] == Status::Gotten) status_[a] = Status::Shown;
}

bool GameState::known(Achievement a) const {
  const auto i = status_.find(a);
  return i != status_.end() && (*i).second != Status::None;
}

GameState::Achievement GameState::alert_needed() const {
  for (const auto& s : status_) {
    if (s.second == Status::Gotten) return s.first;
  }
  return Achievement::Nothing;
}

std::string GameState::Name(Achievement a) {
  switch (a) {
    case Achievement::Nothing:   return "Found a Bug!";
    case Achievement::Missed:    return "The Solution";
    case Achievement::Deathless: return "Kobayashi Maru";
    case Achievement::Dogless:   return "PETA Supporter";
    case Achievement::Split:     return "Junction Malfunction";
    case Achievement::Rejoin:    return "Better Together";
    case Achievement::ThreeWay:  return "Menage a Train";
    default: return "";
  }
}

std::string GameState::Description(Achievement a) {
  switch (a) {
                                       //                        //                        //
    case Achievement::Nothing:   return "I don't know how\n"       "you found this.";
    case Achievement::Missed:    return "What if the train\n"      "doesn't hit anyone?";
    case Achievement::Deathless: return "You managed not to hit\n" "anyone!  Did you cheat?";
    case Achievement::Dogless:   return "You saved all the dogs!\n""But at what cost?";
    case Achievement::Split:     return "Why choose one track\n"   "when you can have both?";
    case Achievement::Rejoin:    return "Maybe splitting up the\n" "train was a bad idea.";
    case Achievement::ThreeWay:  return "It's almost like you are\ntrying to hit people...";
                                       //                        //                        //
    default: return "";
  }
}
