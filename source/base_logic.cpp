//
// Created by rmzs0 on 02.02.2021.
//

#include "base_logic.h"
#include "SFML/Audio.hpp"

[[maybe_unused]] void BL::play_beat_sound(sf::SoundBuffer &buffer) {
    sf::Sound sound;
    sound.setBuffer(buffer);
    sound.play();
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
    combo_status += combo_point;
}

[[maybe_unused]] unsigned long long BL::Game_session::get_combo() const {
    return combo_status;
}

void BL::Game_session::increase_score(unsigned long long score_point,
                                      unsigned long long combo_bonus) {
    score += (score_point * combo_bonus);
}
void BL::Game_session::nullify_score() {
    score = 0;
}

unsigned long long BL::Game_session::get_score() const {
    return score;
}