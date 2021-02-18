#include "map_objects.h"
#include "SFML/System/Time.hpp"

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
    return true; //TODO дописать логику исчезновения
}

bool USO::Aim_circle::check_event(float x,
                                  float y,
                                  BL::Game_session &game_session) {
    if ((x - x_pos) * (x - x_pos) + (y - y_pos) * (y - y_pos) <=
        beat_radius * beat_radius) {
        // TODO дописать штраф или плюшки
        return true;
    }
    return false;
}

void USO::Aim_circle::draw(sf::RenderWindow &window) {
    //Я поправил, тут ты рисовала только внешний круг, в ОСУ рисуется сразу 2: круг по середине и внешняя окружность которая двигается
    sf::CircleShape act_circle(active_circle_radius);
    act_circle.setPosition(x_pos, y_pos);
    act_circle.setFillColor(sf::Color(230, 155, 230));
    sf::CircleShape beat_circle(beat_radius);
    beat_circle.setPosition(x_pos, y_pos);
    beat_circle.setFillColor(sf::Color(255, 255, 255));
    window.draw(act_circle);
    window.draw(beat_circle);
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
