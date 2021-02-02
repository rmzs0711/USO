//
// Created by rmzs0 on 02.02.2021.
//

#ifndef USO_BASE_LOGIC_H
#define USO_BASE_LOGIC_H

#include "SFML/Audio.hpp"
namespace {
const int MAX_HEALTH = 200;
}
namespace BL {

struct Health_bar {
private:
    int health = MAX_HEALTH;

public:
    void increase_health(int);
    void decrease_health(int);
};

struct Combo_status {
private:
    unsigned long long combo = 0;

public:
    void increase_combo(unsigned long long);
    [[maybe_unused]] [[nodiscard]] unsigned long long get_combo() const;
};

struct Score {
private:
    unsigned long long score = 0;
public:
    void increase_score(unsigned long long, unsigned long long);
};

struct Game_session {
    Health_bar health_bar;
};

[[maybe_unused]] void play_beat_sound(sf::SoundBuffer &);

}  // namespace BL

#endif  // USO_BASE_LOGIC_H
