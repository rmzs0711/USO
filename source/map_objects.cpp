#include "map_objects.h"
#include <sstream>
#include "base_logic.h"
namespace {
float get_time_coefficient(sf::Time &start,
                           sf::Time &duration,
                           sf::Time &current) {
    return (current - start) / duration;
}
bool is_click_time(const sf::Time &current_time, const sf::Time &end_time) {
    static sf::Time epsilon = sf::milliseconds(500);
    return end_time - current_time < epsilon;
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
    active_circle_radius =  //Умножаю стартовый радиус на коэф, вижу изменения и
                            //отнимаю от стартового
        active_circle_start_radius -
        active_circle_start_radius *
            get_time_coefficient(start_time, duration_time, current_time);
    return current_time <= start_time + duration_time;
}

bool USO::Aim_circle::check_event(sf::Vector2f mouse_pos,
                                  BL::Game_session &game_session,
                                  sf::Time current_time) {
    if (is_click_time(current_time, start_time + duration_time)) {
        if (is_circle_correct_click(mouse_pos, pos, beat_radius)) {
            game_session.increase_combo(1);  // точно так? //Да я думаю
            game_session.increase_score(100, game_session.get_combo());
            return true;
        }
    }
    game_session.decrease_health(game_session.damage());
    return false;
}

void USO::Aim_circle::draw(sf::RenderWindow &window) {
    sf::CircleShape active_circle(active_circle_radius);
    sf::CircleShape base_circle(beat_radius);
    sf::Text index_of_circle;

    active_circle.setPosition(pos);
    base_circle.setPosition(pos);
    index_of_circle.setPosition(pos);

    active_circle.setFillColor(sf::Color(230, 155, 230));
    base_circle.setFillColor(sf::Color(0, 255, 0));
    index_of_circle.setFillColor(sf::Color::Red);

    //а можно поумнее нарисовать индекс?
    std::ostringstream ostr;
    ostr << index;
    std::string index_str = ostr.str();
    index_of_circle.setString(index_str);

    window.draw(active_circle);
    window.draw(base_circle);
    window.draw(index_of_circle);
}

bool USO::Aim_slider::change_state(sf::Time current_time) {
    if (current_time <= start_time + duration_time) {
        USO::Aim_circle::change_state(current_time);
    } else if (current_time <= start_time + duration_time + move_time) {
        float coef =
            get_time_coefficient(start_time, duration_time, current_time);
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
        game_session.increase_score(1, 25);
        //пока костыль, я не придумал как по умному
        return true;
    }
    return false;
}
void USO::Aim_slider::draw(sf::RenderWindow &window) {
    Aim_circle::draw(window);
}
