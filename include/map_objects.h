#ifndef USO_MAP_OBJECTS_H
#define USO_MAP_OBJECTS_H
#include <memory>
#include "SFML/Graphics.hpp"
#include "SFML/System/Time.hpp"
#include "base_logic.h"

namespace USO {
// enum class Aim_objects { CIRCLE, SLIDER, SPINNER, MUDA };

// enum class Conveyor_objects { NOTE, HOLD_NOTE };

// enum class Bulletproof_objects { SHAPE };

struct Map_object {
protected:
    sf::Time start_time;
    sf::Time duration_time;
    sf::Vector2f pos;
    int index;

    Map_object(const sf::Time &start_time_,
               const sf::Time &duration_time_,
               float x,
               float y,
               int index_)
        : start_time(start_time_),
          duration_time(duration_time_),
          pos(x, y),
          index(index_) {}

public:
    virtual bool change_state(sf::Time) = 0;
    virtual bool check_event(sf::Vector2f,
                             BL::Game_session &game_session,
                             sf::Time) = 0;
    virtual void draw(sf::RenderWindow &window, const sf::Font &font) = 0;
    virtual ~Map_object() = default;
    virtual sf::Time &get_start_time();

    //    virtual sf::Time &get_duration_time();
    //    virtual float &get_x_coord();
    //    virtual float &get_y_coord();
    //    virtual std::shared_ptr<Map_object> clone() = 0;
};

struct Aim_circle : Map_object {
protected:
    float beat_radius;
    float active_circle_radius = 0;
    float active_circle_start_radius;

public:
    Aim_circle(const sf::Time &start_time_,
               const sf::Time &duration_time_,
               float x,
               float y,
               int index_,
               float beat_radius_,
               float active_circle_radius_)
        : Map_object(start_time_, duration_time_, x, y, index_),
          beat_radius(beat_radius_),
          active_circle_start_radius(active_circle_radius_),
          active_circle_radius(active_circle_radius_) {}

    Aim_circle(const Aim_circle &) = default;

    bool change_state(sf::Time current_time) override;
    bool check_event(sf::Vector2f,
                     BL::Game_session &game_session,
                     sf::Time current_time) override;
    void draw(sf::RenderWindow &window, const sf::Font &font) override;
    bool is_valid = true;
};

struct Aim_slider : Aim_circle {
private:
    sf::Vector2f start_pos;
    sf::Vector2f end_pos;
    sf::Time move_time;

public:
    Aim_slider(const sf::Time &start_time_,
               const sf::Time &duration_time_,
               float x,
               float y,
               int index_,
               float beat_radius_,
               float active_circle_start_radius_,
               float x_end_,
               float y_end_,
               sf::Time move_time_)
        : Aim_circle(start_time_,
                     duration_time_,
                     x,
                     y,
                     index_,
                     beat_radius_,
                     active_circle_start_radius_),
          start_pos(x, y),
          end_pos(x_end_, y_end_),
          move_time(move_time_) {}
    bool change_state(sf::Time current_time) override;
    bool check_event(sf::Vector2f,
                     BL::Game_session &game_session,
                     sf::Time current_time) override;
    void draw(sf::RenderWindow &window, const sf::Font &font) override;
    //    std::shared_ptr<Map_object> clone() override;
};

// struct Aim_spinner : Map_object {}; Пока хз какие поля ему дать, как
// отслеживать поворот мыши

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
                  int index_,
                  Conveyor_line line_)
        : Map_object(start_time_,
                     duration_time_,
                     line_.pos.x,
                     line_.pos.y,
                     index_),
          line(line_) {}

    bool change_state(sf::Time current_time) override;
    bool check_event(sf::Vector2f,
                     BL::Game_session &game_session,
                     sf::Time current_time) override;
    void draw(sf::RenderWindow &window, const sf::Font &font) override;
};

}  // namespace USO
#endif  // USO_MAP_OBJECTS_H
