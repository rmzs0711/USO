//
// Created by rmzs0 on 02.02.2021.
//

#include "base_logic.h"
#include "SFML/Audio.hpp"

[[maybe_unused]] void BL::play_beat_sound(sf::SoundBuffer &buffer) {
    //    buffer.loadFromFile(R"(..\source\sounds\click.ogg)");
    sf::Sound sound;
    sound.setBuffer(buffer);
    sound.play();
}

void BL::Health_bar::increase_health(int bonus) {
    health = (health + bonus) % MAX_HEALTH;
}

void BL::Health_bar::decrease_health(int damage) {
    health -= damage;
}

void BL::Combo_status::increase_combo(unsigned long long combo_point) {
    combo += combo_point;
}

[[maybe_unused]] unsigned long long BL::Combo_status::get_combo() const {
    return combo;
}

void BL::Score::increase_score(unsigned long long score_point,
                               unsigned long long combo_bonus) {
    score += (score_point * combo_bonus);
}
