#include "game_state.h"

GameState::GameState() {}

void GameState::achieve(Achievement a) {
  status_.emplace(a, Status::Gotten);
}

void GameState::show(Achievement a) {
  if (status_[a] == Status::Gotten) status_[a] = Status::Shown;
}

GameState::Achievement GameState::alert_needed() const {
  for (const auto& s : status_) {
    if (s.second == Status::Gotten) return s.first;
  }
  return Achievement::Nothing;
}

std::string GameState::Name(Achievement a) {
  switch (a) {
    case Achievement::Nothing: return "Found a Bug!";
    case Achievement::Deathless: return "Kobayashi Maru";
    case Achievement::Dogless: return "PETA Supporter";
    case Achievement::Split: return "Junction Malfunction";
    case Achievement::Rejoin: return "Better Together";
    case Achievement::ThreeWay: return "Menage a Train";
    default: return "";
  }
}

std::string GameState::Description(Achievement a) {
  switch (a) {
    case Achievement::Nothing:   return "I don't know how\nyou found this.";
    case Achievement::Deathless: return "You managed not to hit\nanyone!  Did you cheat?";
    case Achievement::Dogless:   return "You saved all the dogs in\na day, but at what cost?";
    case Achievement::Split:     return "Whoops, the train cars are\nsupposed to stay together.";
    case Achievement::Rejoin:    return "Runaway train crisis averted.";
    case Achievement::ThreeWay:  return "It's almost like you are\ntrying to hit people...";
    default: return "";
  }
}
