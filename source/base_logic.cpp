#include "base_logic.h"
#include <SFML/Graphics/Text.hpp>

[[maybe_unused]] void BL::play_beat_sound(sf::SoundBuffer &buffer) {}

void BL::Game_session::increase_health(int bonus) {
    health += bonus;
    health = health > 200 ? health - (health % MAX_HEALTH) : health;
}

void BL::Game_session::set_health(int health_point) {
    health = health_point;
}

void BL::Game_session::decrease_health(int damage) {
    if (health > 0) {
        health -= damage;
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

void BL::Game_session::set_combo(unsigned long long compo_point) {
    combo_status = compo_point;
}

[[maybe_unused]] unsigned long long BL::Game_session::get_combo() const {
    return combo_status;
}

void BL::Game_session::increase_score(unsigned long long score_point,
                                      unsigned long long combo_bonus) {
    score += (score_point * combo_bonus) / 25;
}

[[nodiscard]] int BL::Game_session::damage() {
    return static_cast<int>(MAX_HEALTH / 10);
}

void BL::Game_session::nullify_score() {
    score = 0;
}

unsigned long long BL::Game_session::get_score() const {
    return score;
}

BL::Game_status BL::Game_session::get_game_status() {
    return game_status;
}
void BL::Game_session::set_game_status(BL::Game_status game_status_) {
    game_status = game_status_;
}

void BL::Game_session::table_of_scores(sf::RenderWindow &window,
                                       sf::Font &font) const {
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(30);
    text.setStyle(sf::Text::Bold);
    text.setString("SCORE: " + std::to_string(get_score()));
    window.draw(text);
    text.setPosition(sf::Vector2f(0.f, 35.f));
    text.setString("COMBO: " + std::to_string(get_combo()));
    window.draw(text);
    text.setPosition(sf::Vector2f(0.f, 70.f));
    text.setString("HEALTH: " + std::to_string(get_health()));
    window.draw(text);
}
