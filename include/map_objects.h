#ifndef USO_MAP_OBJECTS_H
#define USO_MAP_OBJECTS_H
#include <memory>
#include "SFML/Graphics.hpp"
#include "SFML/System/Time.hpp"
#include "base_logic.h"
#include <cmath>
namespace USO {

const int const_circle_beat_radius = 65;
const int const_active_circle_radius = 300;
const sf::Time const_active_circle_duration = sf::seconds(0.7);

const double time_per_pixels = 2;

enum class Aim_objects { CIRCLE, SLIDER, SPINNER, MUDA };

// enum class Conveyor_objects { NOTE, HOLD_NOTE };

// enum class Bulletproof_objects { SHAPE };

struct Map_object {
protected:
    sf::Time start_time;
    sf::Time duration_time;
    sf::Time move_time;
    sf::Vector2f pos;
    Map_object(const sf::Time &start_time_,
               const sf::Time &duration_time_,
               float x,
               float y,
               const sf::Time &move_time_)
        : start_time(start_time_), duration_time(duration_time_), pos(x, y), move_time(move_time_) {
    }

public:
    virtual bool change_state(sf::Time) = 0;
    virtual bool check_event(sf::Vector2f,
                             BL::Game_session &game_session,
                             sf::Time) = 0;
    virtual void draw(sf::RenderWindow &window, const sf::Font &font) = 0;
    virtual ~Map_object() = default;
    virtual sf::Time &get_start_time();
    virtual sf::Time &get_duration_time();
    virtual sf::Vector2f &get_pos();
    virtual sf::Vector2f &get_end_pos() {return pos;}
    virtual std::shared_ptr<Map_object> clone() { return nullptr;}
    virtual void reset() {}
    virtual sf::Time &get_move_time() {
        return move_time;
    }
};

struct Aim_circle : Map_object {
protected:
    float beat_radius;
    float active_circle_radius = 0;
    float active_circle_start_radius;
    bool is_valid = true;
    float time_coef = 1;
public:
    Aim_circle(const sf::Time &start_time_,
               const sf::Time &duration_time_,
               float x,
               float y,
               float beat_radius_,
               float active_circle_radius_,
               const sf::Time &move_time_ = sf::seconds(0))
        : Map_object(start_time_, duration_time_, x, y, move_time_),
          beat_radius(beat_radius_),
          active_circle_start_radius(active_circle_radius_) {}

    //    Aim_circle(const Aim_circle &other_circle)
    //        : Map_object(other_circle.start_time,
    //                     other_circle.duration_time,
    //                     other_circle.pos.x,
    //                     other_circle.pos.y,
    //                     other_circle.index),
    //          beat_radius(other_circle.beat_radius),
    //          active_circle_start_radius(other_circle.active_circle_start_radius),
    //          active_circle_radius(other_circle.active_circle_radius) {}

    Aim_circle(const Aim_circle &) = default;

    bool change_state(sf::Time current_time) override;
    bool check_event(sf::Vector2f,
                     BL::Game_session &game_session,
                     sf::Time current_time) override;
    void draw(sf::RenderWindow &window, const sf::Font &font) override;

    std::shared_ptr<Map_object> clone() override;
    sf::Vector2f &get_end_pos() override;
    void reset() override {
        active_circle_radius = active_circle_start_radius;
    }
};

struct Aim_slider : Aim_circle {
private:
    sf::Vector2f start_pos;
    sf::Vector2f end_pos;
public:
    Aim_slider(const sf::Time &start_time_,
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
    bool change_state(sf::Time current_time) override;
    bool check_event(sf::Vector2f,
                     BL::Game_session &game_session,
                     sf::Time current_time) override;
    void draw(sf::RenderWindow &window, const sf::Font &font) override;
    sf::Vector2f &get_end_pos() override;
    std::shared_ptr<Map_object> clone() override;
    sf::Vector2f& get_start_pos() ;
    void reset() override {
        active_circle_radius = active_circle_start_radius;
        pos = start_pos;
    }
};

struct Aim_spinner : Aim_circle {
private:

    float start_radian;
    float sum_of_radians;
    int change_color;
    std::vector<sf::RectangleShape> Lines;

public:

    Aim_spinner(const sf::Time &start_time_,
                const sf::Time &duration_time_,
                float x_,
                float y_,
                float active_circle_start_radius_
                )
        : Aim_circle(start_time_,
                   duration_time_,
                   x_,
                   y_,
                   10,
                   active_circle_start_radius_),
          sum_of_radians(0),
          start_radian(0),
          change_color(0)
    {
        Lines.resize(8, sf::RectangleShape(sf::Vector2f(active_circle_start_radius_, 4)));
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

    float calc_delta(sf::Vector2f, float&);
    bool change_state(sf::Time current_time) override;
    bool check_event(sf::Vector2f,
                     BL::Game_session &game_session,
                     sf::Time current_time) override;
    void draw(sf::RenderWindow &window, const sf::Font &font) override;
    static bool check_sum_of_radians(float &);
    bool check_event_for_draw(sf::Vector2f);
    std::shared_ptr<Map_object> clone() override;
};

//struct Star : Map_object {
//private:
//    std::vector<sf::RectangleShape> lines;
//
//public:
//    Star(const sf::Time &start_time_,
//         const sf::Time &duration_time_,
//         float x,
//         float y)
//        :
//};


// struct Aim_muda final : Map_object {
// private:
//    float beat_radius;
//    unsigned beat_count;
//
// public:
//    Aim_muda(sf::Time &start_time_,
//             sf::Time &duration_time_,
//             float x,
//             float y,
//             int index_,
//             float beat_radius_,
//             unsigned beat_count_)
//        : Map_object(start_time_, duration_time_, x, y, index_),
//          beat_radius(beat_radius_),
//          beat_count(beat_count_) {}
//};

struct Conveyor_line {
public:
    sf::Vector2f pos;
    sf::Vector2f sizes;
    sf::Vector2f beat_pos;
    sf::Vector2f beat_sizes;
    bool dragged = false;
    int index;
    Conveyor_line(sf::Vector2f pos_, sf::Vector2f sizes_, int index_)
        : pos(pos_), sizes(sizes_), index(index_) {
        beat_sizes.x = sizes.x;
        beat_sizes.y = sizes.y / 5;
        beat_pos.x = pos.x;
        beat_pos.y = pos.y + 4 * sizes.y / 5;
    }

    void draw(sf::RenderWindow &window) const;
};

struct Conveyor_note : Map_object {
private:
    Conveyor_line line;

public:
    Conveyor_note(sf::Time &start_time_,
                  sf::Time &duration_time_,
                  Conveyor_line line_)
        : Map_object(start_time_,
                     duration_time_,
                     line_.pos.x,
                     line_.pos.y, sf::Time()),
          line(line_) {}

    bool change_state(sf::Time current_time) override;
    bool check_event(sf::Vector2f,
                     BL::Game_session &game_session,
                     sf::Time current_time) override;
    void draw(sf::RenderWindow &window, const sf::Font &font) override;

};

}  // namespace USO
#endif  // USO_MAP_OBJECTS_H
