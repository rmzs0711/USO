#include "map_objects.h"
#include "base_logic.h"
#include <sstream>

sf::Time &USO::Map_object::get_start_time() {
    return start_time;
}

sf::Time &USO::Map_object::get_duration_time() {
    return duration_time;
}

float &USO::Map_object::get_x_coord() {
    return x_pos;
}

float &USO::Map_object::get_y_coord() {
    return y_pos;
}

bool USO::Aim_circle::change_state() {
    active_circle_radius -= active_circle_shift;
<<<<<<< HEAD
    if (active_circle_radius >= beat_radius) {
        return true;  // TODO логика исчезновения (Айгерим)
    }
    // TODO нарисовать исчезновение и вызвать конструктор?
    return false;
=======

>>>>>>> origin/Rama
}

bool USO::Aim_circle::check_event(float x,
                                  float y,
                                  BL::Game_session &game_session) {
    if ((x - x_pos) * (x - x_pos) + (y - y_pos) * (y - y_pos) <=
        beat_radius * beat_radius) {
<<<<<<< HEAD
        game_session.increase_combo(1); // TODO а как?
        game_session.increase_score(10, 10); // TODO как посчитать бонус?
=======

>>>>>>> origin/Rama
        return true;
    }
    game_session.decrease_health(game_session.damage());
    return false;
}

void USO::Aim_circle::draw(sf::RenderWindow &window) {
<<<<<<< HEAD
    sf::CircleShape active_circle(active_circle_radius);
    sf::CircleShape base_circle(beat_radius);
    sf::Text index_of_circle;

    active_circle.setPosition(x_pos, y_pos);
    base_circle.setPosition(x_pos, y_pos);
    index_of_circle.setPosition(x_pos, y_pos);

    active_circle.setFillColor(sf::Color(230, 155, 230));
    base_circle.setFillColor(sf::Color(0, 255, 0));
    index_of_circle.setFillColor(sf::Color::Red);

    //а можно поумнее?
    std::ostringstream ostr;
    ostr << index;
    std::string index_str = ostr.str();
    index_of_circle.setString(index_str);

    window.draw(active_circle);
    window.draw(base_circle);
    window.draw(index_of_circle);
=======
    
>>>>>>> origin/Rama
}

bool USO::Aim_slider::change_state() {
    if (beat_radius < active_circle_radius) {
        USO::Aim_circle::change_state();
    } else if (dist_pow_2 > 0) {
        x_pos += x_shift;
        y_pos += y_shift;
        dist_pow_2 -= (x_shift * x_shift + y_shift * y_shift);
    } else {
        return false;
    }
    return true;
}
bool USO::Aim_slider::check_event(float x,
                                  float y,
                                  BL::Game_session &game_session) {
    if (beat_radius >= active_circle_radius) {
        if (USO::Aim_circle::check_event(x, y, game_session)) {
            //TODO По идее за круг уже дадут плюшки, тут я их просто урежу чутка,
            //чтобы сильного навара в очках не было
            return true;
        }
    }
    return false;
}
void USO::Aim_slider::draw(sf::RenderWindow &window) {
    Aim_circle::draw(window);
}
