#include "base_logic.h"
#include "SFML/Audio.hpp"

[[maybe_unused]] void BL::play_beat_sound(sf::SoundBuffer &buffer) {

}

void BL::Game_session::increase_health(int bonus) {
    health += bonus;
    health = health - (health % MAX_HEALTH);
}

void BL::Game_session::decrease_health(int damage) {
    health -= damage;
    if (game_status == Game_status::ACTION && health < 0) {
        game_status = Game_status::DEFEAT;
    }
}

int BL::Game_session::get_health() const {
    return health;
}

void BL::Game_session::increase_combo(unsigned long long combo_point) {
    if (is_combo_locked) {
        return;
    }
    combo_status += combo_point;
}

[[maybe_unused]] unsigned long long BL::Game_session::get_combo() const {
    return combo_status;
}

void BL::Game_session::increase_score(unsigned long long score_point,
                                      unsigned long long combo_bonus) {
    if (is_score_locked) {
        return;
    }
    /* возможно, стоит вынести 25 в константу и дать ей такое название, чтобы
     * было понятно, что она означает */
    score += (score_point * combo_bonus) / 25;
}

[[nodiscard]] int BL::Game_session::damage() const {
    /* 0.05 тоже стоит вынести в константу. Как минимум потому, что так будет
     * удобнее править баланс. */
    return static_cast<int>(0.05 * game_complexity * MAX_HEALTH);
}

/* Note: reset кажется более подходящим словом, но тут уж как кому нравится */
void BL::Game_session::nullify_score() {
    score = 0;
}

unsigned long long BL::Game_session::get_score() const {
    return score;
}

void BL::Game_session::pause_session() {
    game_status = Game_status::PAUSE;
}

void BL::Game_session::lock_combo() {
    is_combo_locked = true;
}
void BL::Game_session::unlock_combo() {
    is_combo_locked = false;
}

void BL::Game_session::lock_score() {
    is_score_locked = true;
}
void BL::Game_session::unlock_score() {
    is_score_locked = false;
}
