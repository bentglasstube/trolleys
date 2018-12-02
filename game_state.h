#pragma once

class GameState {
  public:

    // achievements
    // 0 = not achieved
    // 1 = achieved, not alerted
    // 2 = achieved, alerted
    int deathless = 0;
    int dogless = 0;
    int split = 0;
    int rejoin = 0;
    int three_way = 0;
};
