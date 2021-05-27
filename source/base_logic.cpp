#include "base_logic.h"
#include <SFML/Graphics/Text.hpp>
#include "SFML/Audio.hpp"

[[maybe_unused]] void BL::play_beat_sound(sf::SoundBuffer &buffer) {}

void BL::Game_session::increase_health(int bonus) {
    health += bonus;
    health = health > 200 ? health - (health % MAX_HEALTH) : health;
}

void BL::Game_session::decrease_health(int damage) {
    if (health > 0) {
        health -= damage;
    }
    //    if (game_status == Game_status::ACTION && health < 0) {
    //        game_status = Game_status::DEFEAT;
    //    }
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
    //    if (is_score_locked) {
    //        return;
    //    }
    score += (score_point * combo_bonus) / 25;
}

[[nodiscard]] int BL::Game_session::damage() const {
    return static_cast<int>(0.05 * game_complexity * MAX_HEALTH);
}

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

void BL::Game_session::table_of_scores(sf::RenderWindow &window,
                                       sf::Font &font) {
    /*sf::RectangleShape table;
    table.setSize(sf::Vector2f(250.f, 120.f));
    table.setFillColor(sf::Color::Blue);
    table.setOutlineThickness(5.f);
    table.setOutlineColor(sf::Color);
    window.draw(table);*/
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(30);
    //    text.setFillColor(sf::Color::White);
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