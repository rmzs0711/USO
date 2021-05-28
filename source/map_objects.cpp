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
    static sf::Time epsilon = sf::milliseconds(700);
    return end_time - current_time < epsilon ||
           current_time - end_time < epsilon;
}
sf::Vector2f fix_circle_pos(const sf::Vector2f &pos, const float &radius) {
    return sf::Vector2f(pos.x - radius, pos.y - radius);
}

}  // namespace

sf::Time &USO::Map_object::get_start_time() {
    return start_time;
}

sf::Time &USO::Map_object::get_duration_time() {
    return duration_time;
}

const sf::Vector2f &USO::Map_object::get_pos() const {
    return pos;
}
const sf::Vector2f &USO::Map_object::get_end_pos() const {
    return get_pos();
}

const sf::Time &USO::Map_object::get_move_time() const {
    return move_time;
}
void USO::Map_object::reset() {}
USO::Map_object::Map_object(const sf::Time &start_time_,
                            const sf::Time &duration_time_,
                            float x,
                            float y,
                            const sf::Time &move_time_)
    : start_time(start_time_),
      duration_time(duration_time_),
      pos(x, y),
      move_time(move_time_) {}
const void USO::Map_object::set_move_time(const sf::Time &new_time) {
    move_time = new_time;
}

bool USO::Aim_circle::change_state(const sf::Time &current_time) {
    if (current_time <= start_time + duration_time) {
        time_coef =
            get_time_coefficient(start_time, duration_time, current_time);
        active_circle_radius =  //Умножаю стартовый радиус на коэф, вижу
                                //изменения и отнимаю от стартового
            active_circle_start_radius -
            (active_circle_start_radius - beat_radius) * time_coef;
        return true;
    } else {
        is_valid = false;
    }
    return false;
}

bool USO::Aim_circle::check_event(const sf::Vector2f &mouse_pos,
                                  BL::Game_session &game_session,
                                  const sf::Time &current_time) {
    if (is_circle_correct_click(sf::Vector2f(mouse_pos), pos, beat_radius)) {
        if (is_click_time(current_time, start_time + duration_time)) {
            game_session.increase_combo(1);  // точно так? //Да я думаю
            game_session.increase_score(100, game_session.get_combo());
            game_session.increase_health(100);
            return true;
        } else {
            is_valid = false;
            game_session.decrease_health(game_session.damage());
        }
    }
    return false;
}

void USO::Aim_circle::draw(sf::RenderWindow &window, const sf::Font &font) {
    if (is_valid) {
        sf::CircleShape active_circle(active_circle_radius);
        sf::CircleShape base_circle(beat_radius);
        base_circle.setOrigin(beat_radius, beat_radius);
        active_circle.setPosition(fix_circle_pos(pos, active_circle_radius));
        if (active_circle_radius <= beat_radius) {
            active_circle.setRadius(beat_radius);
            active_circle.setPosition(fix_circle_pos(pos, beat_radius));
        }
        base_circle.setPosition(pos);

        active_circle.setFillColor(sf::Color::Transparent);
        active_circle.setOutlineThickness(20);
        active_circle.setOutlineColor(sf::Color::Magenta);
        base_circle.setFillColor(
            sf::Color(100, 5, 94, sf::Uint8(255 * time_coef)));
        base_circle.setOutlineThickness(2);
        base_circle.setOutlineColor(sf::Color::Magenta);

        window.draw(base_circle);
        window.draw(active_circle);
    } else {
        sf::Text denied;
        denied.setString("X");
        denied.setFont(font);
        denied.setFillColor(sf::Color::Red);
        denied.setCharacterSize(65);
        denied.setPosition(pos -
                           sf::Vector2f((float)denied.getCharacterSize() / 3,
                                        (float)denied.getCharacterSize() / 2));
        window.draw(denied);
    }
}
std::shared_ptr<USO::Map_object> USO::Aim_circle::clone() {
    return std::make_shared<Aim_circle>(Aim_circle(*this));
}
bool USO::Aim_circle::is_circle_correct_click(const sf::Vector2f &mouse,
                                              const sf::Vector2f &center,
                                              const float &radius) {
    return (mouse.x - center.x) * (mouse.x - center.x) +
               (mouse.y - center.y) * (mouse.y - center.y) <=
           radius * radius;
}
USO::Aim_circle::Aim_circle(const sf::Time &start_time_,
                            const sf::Time &duration_time_,
                            float x,
                            float y,
                            float beat_radius_,
                            float active_circle_radius_,
                            const sf::Time &move_time_)
    : Map_object(start_time_, duration_time_, x, y, move_time_),
      beat_radius(beat_radius_),
      active_circle_start_radius(active_circle_radius_) {}
sf::Vector2f *USO::Aim_circle::get_end_pos_ptr() {
    return &pos;
}
void USO::Aim_circle::reset() {
    active_circle_radius = active_circle_start_radius;
}

bool USO::Aim_slider::change_state(const sf::Time &current_time) {
    if (current_time <= start_time + duration_time) {
        USO::Aim_circle::change_state(current_time);
    } else if (current_time <= start_time + duration_time + move_time) {
        is_valid = true;
        float coef = get_time_coefficient(start_time + duration_time, move_time,
                                          current_time);
        active_circle_radius = beat_radius;
        pos.x = start_pos.x + (end_pos.x - start_pos.x) * coef;
        pos.y = start_pos.y + (end_pos.y - start_pos.y) * coef;
    } else {
        return false;
    }
    return true;
}
bool USO::Aim_slider::check_event(const sf::Vector2f &mouse_pos,
                                  BL::Game_session &game_session,
                                  const sf::Time &current_time) {
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

    sf::ConvexShape track1;
    track1.setPointCount(4);
    sf::ConvexShape track2;
    track2.setPointCount(4);
    float delta_x = beat_radius;
    float delta_y = 0;
    if (std::abs(pos.x - end_pos.x) > std::abs(pos.y - end_pos.y)) {
        delta_x = 0;
        delta_y = beat_radius;
    }

    track1.setPoint(0, sf::Vector2<float>(pos.x - delta_x, pos.y - delta_y));
    track1.setPoint(1, sf::Vector2<float>(pos.x - delta_x + 0.5, pos.y - delta_y + 0.5));
    track1.setPoint(
        2, sf::Vector2<float>(end_pos.x - delta_x, end_pos.y - delta_y));
    track1.setPoint(
        3, sf::Vector2<float>(end_pos.x - delta_x - 0.5, end_pos.y - delta_y - 0.5));

    track2.setPoint(0, sf::Vector2<float>(pos.x + delta_x, pos.y + delta_y));
    track2.setPoint(1, sf::Vector2<float>(pos.x + delta_x + 0.5, pos.y + delta_y + 0.5));
    track2.setPoint(
        2, sf::Vector2<float>(end_pos.x + delta_x, end_pos.y + delta_y));
    track2.setPoint(
        3, sf::Vector2<float>(end_pos.x + delta_x - 0.5, end_pos.y + delta_y - 0.5));

    track1.setFillColor(sf::Color(253, 151, 114));
    track1.setOutlineThickness(1.f);
    track1.setOutlineColor(sf::Color::White);
    track2.setFillColor(sf::Color(253, 151, 114));
    track2.setOutlineThickness(1.f);
    track2.setOutlineColor(sf::Color::White);

    window.draw(track1);
    window.draw(track2);
    window.draw(target_circle);
    Aim_circle::is_valid = true;
    Aim_circle::draw(window, font);
}

float USO::Aim_spinner::calc_delta(const sf::Vector2f &cur_vector,
                                   float &prev_radian) {
    float x_pos = cur_vector.x - get_pos().x;
    float y_pos = get_pos().y - cur_vector.y;
    if (x_pos * x_pos + y_pos * y_pos >
        active_circle_radius * active_circle_radius) {
        return 0;
    }
    float radian = x_pos / std::hypot(y_pos, x_pos);
    radian = std::acos(radian);
    if (y_pos > 2.f * (float)M_PI) {
        radian = 2.f * (float)M_PI - radian;
    }
    float result = std::abs(radian - prev_radian);
    prev_radian = radian;
    if (result > 0.78) {
        return 0;
    }
    return result;
}

bool USO::Aim_spinner::check_sum_of_radians(float &cur_sum) {
    if (cur_sum >= 2.f * (float)M_PI) {
        cur_sum = 0;
        return true;
    }
    return false;
}

bool USO::Aim_spinner::change_state(const sf::Time &current_time) {
    return Aim_circle::change_state(current_time);
}

bool USO::Aim_spinner::check_event(const sf::Vector2f &mouse_pos,
                                   BL::Game_session &game_session,
                                   const sf::Time &current_time) {
    if (current_time <= start_time + duration_time) {
        sum_of_radians += calc_delta(mouse_pos, start_radian);
        if (check_sum_of_radians(sum_of_radians)) {
            game_session.increase_health(20);
            game_session.increase_combo(1);
            game_session.increase_score(100, game_session.get_combo());
        }
        return true;
    }
    return false;
}

bool USO::Aim_spinner::check_event_for_draw(const sf::Vector2f &mouse_pos) {
    float epsilon = 20.f;
    float x_pos = std::abs(mouse_pos.x - get_pos().x);
    float y_pos = std::abs(mouse_pos.y - get_pos().y);
    if ((std::abs(x_pos - y_pos) < epsilon || x_pos < epsilon ||
         y_pos < epsilon) &&
        x_pos * x_pos + y_pos * y_pos <=
            active_circle_radius * active_circle_radius) {
        return true;
    }
    return false;
}

void USO::Aim_spinner::draw(sf::RenderWindow &window, const sf::Font &font) {
    for (int i = 0; i < 8; i++) {
        change_color++;
        if (change_color == 105) {
            Lines[i].setFillColor(Lines[(i + change_color) % 8].getFillColor());
            change_color = 0;
        }
        Lines[i].setSize(sf::Vector2f(active_circle_radius, 4));
        window.draw(Lines[i]);
    }

    if (USO::Aim_spinner::check_event_for_draw(
            (sf::Vector2f)sf::Mouse::getPosition())) {
        sf::CircleShape circle;
        circle.setRadius(10.f);
        circle.setPosition((float)sf::Mouse::getPosition().x - 5,
                           (float)sf::Mouse::getPosition().y - 5);
        circle.setFillColor(sf::Color::Magenta);
        window.draw(circle);
    }
    Aim_circle::draw(window, font);
}
const sf::Vector2f &USO::Aim_slider::get_end_pos() const {
    return end_pos;
}
sf::Vector2f *USO::Aim_slider::get_end_pos_ptr() {
    return &end_pos;
}
std::shared_ptr<USO::Map_object> USO::Aim_slider::clone() {
    return std::make_shared<Aim_slider>(Aim_slider(*this));
}
void USO::Aim_slider::reset() {
    active_circle_radius = active_circle_start_radius;
    pos = start_pos;
}

std::shared_ptr<USO::Map_object> USO::Aim_spinner::clone() {
    return std::make_shared<Aim_spinner>(Aim_spinner(*this));
}
USO::Aim_spinner::Aim_spinner(const sf::Time &start_time_,
                              const sf::Time &duration_time_,
                              float x_,
                              float y_,
                              float active_circle_start_radius_)
    : Aim_circle(start_time_,
                 duration_time_,
                 x_,
                 y_,
                 10,
                 active_circle_start_radius_),
      sum_of_radians(0),
      start_radian(0),
      change_color(0) {
    Lines.resize(
        8, sf::RectangleShape(sf::Vector2f(active_circle_start_radius_, 4)));
    for (int i = 0; i < 8; i++) {
        Lines[i].setPosition(x_, y_);
        Lines[i].rotate((float)i * 45);
    }
    Lines[0].setFillColor(sf::Color(193, 197, 54));
    Lines[1].setFillColor(sf::Color(224, 128, 75));
    Lines[2].setFillColor(sf::Color(47, 227, 253));
    Lines[3].setFillColor(sf::Color(253, 227, 47));
    Lines[4].setFillColor(sf::Color(253, 47, 237));
    Lines[5].setFillColor(sf::Color(219, 81, 171));
    Lines[6].setFillColor(sf::Color(109, 182, 190));
    Lines[7].setFillColor(sf::Color(193, 199, 101));
}

const sf::Vector2f &USO::Aim_slider::get_start_pos() const {
    return start_pos;
}
USO::Aim_slider::Aim_slider(const sf::Time &start_time_,
                            const sf::Time &duration_time_,
                            float x,
                            float y,
                            float beat_radius_,
                            float active_circle_start_radius_,
                            float x_end_,
                            float y_end_,
                            const sf::Time &move_time_)
    : Aim_circle(start_time_,
                 duration_time_,
                 x,
                 y,
                 beat_radius_,
                 active_circle_start_radius_,
                 move_time_),
      start_pos(x, y),
      end_pos(x_end_, y_end_) {}

bool USO::Conveyor_note::change_state(const sf::Time &current_time) {
    if (current_time <= start_time + duration_time) {
        pos.y =
            line.pos.y - line.beat_sizes.y +
            (-line.pos.y + line.beat_sizes.y + line.beat_pos.y) *
                get_time_coefficient(start_time, duration_time, current_time);
        return true;
    }
    is_valid = false;
    return false;
}

bool USO::Conveyor_note::check_event(const sf::Vector2f &mouse_pos,
                                     BL::Game_session &game_session,
                                     const sf::Time &current_time) {
    if (is_note_correct_click(mouse_pos, pos, line)) {
        if (is_click_time(current_time, start_time + duration_time)) {
            game_session.increase_combo(1);
            game_session.increase_score(100, game_session.get_combo());
            game_session.increase_health(100);
            return true;
        } else {
            is_valid = false;
            game_session.decrease_health(game_session.damage());
        }
    }
    return false;
}
void USO::Conveyor_note::draw(sf::RenderWindow &window, const sf::Font &font) {
    sf::RectangleShape rectangle(line.beat_sizes);
    rectangle.setPosition(pos);
    if (is_valid) {
        rectangle.setFillColor(sf::Color::Blue);
    } else {
        rectangle.setFillColor(sf::Color::Red);
    }
    rectangle.setOutlineColor(sf::Color::White);
    window.draw(rectangle);
}
sf::Vector2f *USO::Conveyor_note::get_end_pos_ptr() {
    return &pos;
}

std::shared_ptr<USO::Map_object> USO::Conveyor_note::clone() {
    return std::make_shared<Conveyor_note>(Conveyor_note(*this));
}
bool USO::Conveyor_note::is_note_correct_click(
    const sf::Vector2f &mouse_pos,
    const sf::Vector2f &pos,
    const USO::Conveyor_line &line_) {
    if (mouse_pos == line_.beat_pos && pos.y >= line_.beat_pos.y - 100 &&
        pos.y <= line_.beat_pos.y + 100) {
        return true;
    }
    return false;
}
USO::Conveyor_note::Conveyor_note(const sf::Time &start_time_,
                                  const sf::Time &duration_time_,
                                   USO::Conveyor_line &line_)
    : Map_object(start_time_,
                 duration_time_,
                 line_.pos.x,
                 line_.pos.y,
                 sf::Time()),
      line(line_) {}
const USO::Conveyor_line &USO::Conveyor_note::get_line() const {
    return line;
}

void USO::Conveyor_line::draw(sf::RenderWindow &window) const {
    sf::RectangleShape fall(sizes);
    fall.setPosition(pos);
    fall.setFillColor(sf::Color(234, 243, 0, 40));
    fall.setOutlineColor(sf::Color::White);
    fall.setOutlineThickness(1);
    window.draw(fall);

    sf::RectangleShape beat_rectangle(beat_sizes);
    beat_rectangle.setPosition(beat_pos);
    if (dragged) {
        if (!missed) {
            beat_rectangle.setFillColor(sf::Color::Cyan);
        } else {
            beat_rectangle.setFillColor(sf::Color::Red);
        }
    } else {
        beat_rectangle.setFillColor(sf::Color::Green);
    }
    beat_rectangle.setOutlineColor(sf::Color::White);
    beat_rectangle.setOutlineThickness(1);
    window.draw(beat_rectangle);
}
