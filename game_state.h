#pragma once

#include <string>
#include <unordered_map>

#include "util.h"

class GameState {
  public:

    enum class Achievement {
      Nothing,
      Missed,
      Deathless,
      Dogless,
      Split,
      Rejoin,
      ThreeWay,
    };

    GameState();

    void achieve(Achievement a);
    void show(Achievement a);
    bool known(Achievement a) const;
    Achievement alert_needed() const;

    static std::string Name(Achievement a);
    static std::string Description(Achievement a);

  private:

    enum class Status { None, Gotten, Shown };

    std::unordered_map<Achievement, Status, Util::CastHash<Achievement>>  status_;
};
