#ifndef USO_BASE_LOGIC_H
#define USO_BASE_LOGIC_H

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include "SFML/Audio.hpp"

namespace {
const int MAX_HEALTH = 200;
const int MIN_GAME_COMPLEXITY = 1;
}  // namespace

namespace BL {
enum class Game_status { ACTION, PAUSE, VICTORY, DEFEAT };

struct Game_session {
private:
    Game_status game_status;
    int health;
    int game_complexity;
    unsigned long long score;
    unsigned long long combo_status;
    bool is_combo_locked = false;

public:
    Game_session()
        : game_status(Game_status::ACTION),
          health(MAX_HEALTH),
          game_complexity(MIN_GAME_COMPLEXITY),
          score(0),
          combo_status(1) {}

    void increase_score(unsigned long long, unsigned long long);
    [[maybe_unused]] void nullify_score();
    [[nodiscard]] unsigned long long get_score() const;

    void increase_combo(unsigned long long);
    [[nodiscard]] unsigned long long get_combo() const;
    void set_combo(unsigned long long);

    void increase_health(int);
    void decrease_health(int);
    void set_health(int);
    [[nodiscard]] int get_health() const;
    [[nodiscard]] static int damage() ;
    void set_game_status(BL::Game_status);
    Game_status get_game_status();
    void table_of_scores(sf::RenderWindow &window, sf::Font &font) const;
};

[[maybe_unused]] void play_beat_sound(sf::SoundBuffer &);

}  // namespace BL

#endif  // USO_BASE_LOGIC_H
