#include "map_objects.h"
#include <sstream>
#include "base_logic.h"
namespace {
float get_time_coefficient(const sf::Time &start,
                           const sf::Time &duration,
                           const sf::Time &current) {
    return (current - start) / duration;
}
bool is_click_time(const sf::Time &current_time, const sf::Time &end_time) {
    static sf::Time epsilon = sf::milliseconds(200);
    return end_time - current_time < epsilon;
}
sf::Vector2f fix_circle_pos(const sf::Vector2f &pos, const float &radius) {
    return sf::Vector2f(pos.x - radius, pos.y - radius);
}
bool is_circle_correct_click(const sf::Vector2f &mouse,
                             const sf::Vector2f &center,
                             const float &radius) {
    return (mouse.x - center.x) * (mouse.x - center.x) +
               (mouse.y - center.y) * (mouse.y - center.y) <=
           radius * radius;
}
}  // namespace

sf::Time &USO::Map_object::get_start_time() {
    return start_time;
}

sf::Time &USO::Map_object::get_duration_time() {
    return duration_time;
}

float &USO::Map_object::get_x_coord() {
    return pos.x;
}

float &USO::Map_object::get_y_coord() {
    return pos.y;
}

bool USO::Aim_circle::change_state(sf::Time current_time) {
    if (current_time <= start_time + duration_time + sf::milliseconds(100)) {
        active_circle_radius =  //Умножаю стартовый радиус на коэф, вижу
                                //изменения и отнимаю от стартового
            active_circle_start_radius -
            (active_circle_start_radius - beat_radius) *
                get_time_coefficient(start_time, duration_time, current_time);
        return true;
    }
    return false;
}

bool USO::Aim_circle::check_event(sf::Vector2f mouse_pos,
                                  BL::Game_session &game_session,
                                  sf::Time current_time) {
    if (is_circle_correct_click(mouse_pos, pos, beat_radius)) {
        if (is_click_time(current_time, start_time + duration_time)) {
            game_session.increase_combo(1);  // точно так? //Да я думаю
            game_session.increase_score(100, game_session.get_combo());
            game_session.increase_health(100);
            return true;
        } else {
            is_valid = false;

        }
    }
    //    game_session.decrease_health(game_session.damage());
    return false;
}

void USO::Aim_circle::draw(sf::RenderWindow &window, const sf::Font &font) {
    if (is_valid) {
        sf::CircleShape active_circle(active_circle_radius);
        sf::CircleShape base_circle(beat_radius);
        sf::Text index_of_circle;


        active_circle.setPosition(fix_circle_pos(pos, active_circle_radius));
        if (active_circle_radius <= beat_radius) {
            active_circle.setRadius(beat_radius);
            active_circle.setPosition(fix_circle_pos(pos, beat_radius));
        }
        base_circle.setPosition(fix_circle_pos(pos, beat_radius));
        index_of_circle.setCharacterSize(42);
        index_of_circle.setPosition(fix_circle_pos(pos, (float)index_of_circle.getCharacterSize() / 2));
        index_of_circle.setFont(font);


        active_circle.setFillColor(sf::Color::Transparent);
        active_circle.setOutlineThickness(10);
        active_circle.setOutlineColor(sf::Color(230, 0, 0));
        //    base_circle.setFillColor(sf::Color::Transparent);
        base_circle.setFillColor(sf::Color(204, 51, 51));
        base_circle.setOutlineThickness(10);
        base_circle.setOutlineColor(sf::Color::White);
        index_of_circle.setFillColor(sf::Color::White);
        index_of_circle.setOutlineColor(sf::Color::White);

        index_of_circle.setString(std::to_string(index % 5 + 1));
        window.draw(base_circle);
        window.draw(active_circle);
        window.draw(index_of_circle);
    } else {
    }
}
std::shared_ptr<USO::Map_object> USO::Aim_circle::clone() {
    return std::make_shared<Aim_circle>(Aim_circle(*this));
}

bool USO::Aim_slider::change_state(sf::Time current_time) {
    if (current_time <= start_time + duration_time) {
        USO::Aim_circle::change_state(current_time);
    } else if (current_time <= start_time + duration_time + move_time) {
        float coef = get_time_coefficient(start_time + duration_time, move_time,
                                          current_time);
        pos.x = start_pos.x + (end_pos.x - start_pos.x) * coef;
        pos.y = start_pos.y + (end_pos.y - start_pos.y) * coef;
    } else {
        return false;
    }
    return true;
}
bool USO::Aim_slider::check_event(sf::Vector2f mouse_pos,
                                  BL::Game_session &game_session,
                                  sf::Time current_time) {
    if (current_time >= start_time + duration_time &&
        is_click_time(current_time, start_time + duration_time) &&
        is_circle_correct_click(mouse_pos, pos, beat_radius)) {
        game_session.increase_score(100, game_session.get_combo());
        return true;
    }
    return false;
}
void USO::Aim_slider::draw(sf::RenderWindow &window, const sf::Font &font) {
    sf::CircleShape target_circle(beat_radius);
    target_circle.setOutlineThickness(5.f);
    target_circle.setOutlineColor(sf::Color::White);
    target_circle.setPosition(fix_circle_pos(end_pos, beat_radius));
    target_circle.setFillColor(sf::Color(253, 151, 114));

    sf::ConvexShape track;
    track.setPointCount(4);
    float delta_x = beat_radius;
    float delta_y = 0;
    if (std::abs(pos.x - end_pos.x) > std::abs(pos.y - end_pos.y)) {
        delta_x = 0;
        delta_y = beat_radius;
    }
    track.setPoint(0, sf::Vector2<float>(pos.x - delta_x, pos.y - delta_y));
    track.setPoint(1, sf::Vector2<float>(pos.x + delta_x, pos.y + delta_y));
    track.setPoint(
        2, sf::Vector2<float>(end_pos.x + delta_x, end_pos.y + delta_y));
    track.setPoint(
        3, sf::Vector2<float>(end_pos.x - delta_x, end_pos.y - delta_y));
    track.setFillColor(sf::Color(253, 151, 114));
    track.setOutlineThickness(5.f);
    track.setOutlineColor(sf::Color::White);

    //костыль для закраски
    sf::ConvexShape track2;
    track2.setPointCount(4);
    track2.setPoint(0, sf::Vector2<float>(pos.x - delta_x, pos.y - delta_y));
    track2.setPoint(1, sf::Vector2<float>(pos.x + delta_x, pos.y + delta_y));
    track2.setPoint(
        2, sf::Vector2<float>(end_pos.x + delta_x, end_pos.y + delta_y));
    track2.setPoint(
        3, sf::Vector2<float>(end_pos.x - delta_x, end_pos.y - delta_y));
    track2.setFillColor(sf::Color(253, 151, 114));
    track2.setOutlineThickness(0.f);
    track2.setOutlineColor(sf::Color::Transparent);

    window.draw(track);
    window.draw(target_circle);
    window.draw(track2);
    Aim_circle::draw(window, font);
}

std::shared_ptr<USO::Map_object> USO::Aim_slider::clone() {
    return std::make_shared<Aim_circle>(Aim_slider(*this));
}
