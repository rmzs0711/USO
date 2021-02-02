#ifndef USO_BASE_LOGIC_H
#define USO_BASE_LOGIC_H

#include "SFML/Audio.hpp"

namespace {
const int MAX_HEALTH = 200;
}  // namespace

namespace BL {
enum class Game_status {
    ACTION,
    PAUSE,
    VICTORY,
    DEFEAT,
};

struct Game_session {
private:
    Game_status game_status;
    int health;
    unsigned long long score;
    unsigned long long combo_status;

public:
    Game_session()
        : game_status(Game_status::ACTION),
          health(MAX_HEALTH),
          score(0),
          combo_status(0) {}

    void increase_score(unsigned long long, unsigned long long);
    void nullify_score();
    [[nodiscard]] unsigned long long get_score() const;

    void increase_combo(unsigned long long);
    [[nodiscard]] unsigned long long get_combo() const;

    void increase_health(int);
    void decrease_health(int);
    [[nodiscard]] int get_health() const;

    Game_status get_game_status(/*map*/) {
        //        if (map == ended) { TODO
        //            game_status = Game_status::VICTORY;
        //        }
        return game_status;
    }
    void pause_session();
};

[[maybe_unused]] void play_beat_sound(sf::SoundBuffer &);

}  // namespace BL

#endif  // USO_BASE_LOGIC_H
