#pragma once

#include <unordered_map>

class GameState {
  public:

    enum class Achievement {
      Nothing,
      Deathless,
      Dogless,
      Split,
      Rejoin,
      ThreeWay,
    };

    GameState();

    void achieve(Achievement a);
    void show(Achievement a);
    Achievement alert_needed() const;

    static std::string Name(Achievement a);
    static std::string Description(Achievement a);

  private:

    enum class Status { None, Gotten, Shown };

    std::unordered_map<Achievement, Status>  status_;
};
