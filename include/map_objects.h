#ifndef USO_MAP_OBJECTS_H
#define USO_MAP_OBJECTS_H

namespace USO {
enum class Aim_objects { CIRCLE, SLIDER, SPINNER, MUDA };

enum class Conveyor_objects { NOTE, HOLD_NOTE };

enum class Bulletproof_objects {
    SHAPE,
};

struct Map_object {
private:
    sf::Time start_time;
    sf::Time duration_time;
    float x_pos;
    float y_pos;

protected:
    Map_object(sf::Time &start_time_,
               sf::Time &duration_time_,
               float x,
               float y)
        : start_time(start_time_),
          duration_time(duration_time_),
          x_pos(x),
          y_pos(y) {}
    virtual ~Map_object() = default;
};

struct Aim_circle final : Map_object {
private:
    float beat_radius;
    float active_circle_start_radius;
    float active_circle_radius_shift;

public:
    Aim_circle(sf::Time &start_time_,
               sf::Time &duration_time_,
               float x,
               float y,
               float beat_radius_,
               float active_circle_start_radius_,
               float active_circle_radius_shift_)
        : Map_object(start_time_, duration_time_, x, y),
          beat_radius(beat_radius_),
          active_circle_start_radius(active_circle_start_radius_),
          active_circle_radius_shift(active_circle_radius_shift_) {}
    // TODO Logic
};

//struct Aim_slider : Aim_circle {
//private:
//    // TODO traectory
//public:
//    Aim_slider(sf::Time &start_time_,
//               sf::Time &duration_time_,
//               float x,
//               float y,
//               float beat_radius_,
//               float active_circle_start_radius_,
//               float active_circle_radius_shift_)
//        : Aim_circle(start_time_,
//                     duration_time_,
//                     x,
//                     y,
//                     beat_radius_,
//                     active_circle_start_radius_,
//                     active_circle_radius_shift_) {}
//};

// struct Aim_spinner : Map_object {}; Пока хз какие поля ему дать, как
// отслеживать поворот мыши

struct Aim_muda final : Map_object {
private:
    float beat_radius;
    unsigned beat_count;

public:
    Aim_muda(sf::Time &start_time_,
             sf::Time &duration_time_,
             float x,
             float y,
             float beat_radius_,
             unsigned beat_count_)
        : Map_object(start_time_, duration_time_, x, y),
          beat_radius(beat_radius_),
          beat_count(beat_count_) {}
};

enum class Conveyor_note_key_position { D, F, J, K };

struct Conveyor_note : Map_object {
private:
    Conveyor_note_key_position position;

public:
    Conveyor_note(sf::Time &start_time_,
                  sf::Time &duration_time_,
                  float x,
                  float y,
                  Conveyor_note_key_position position_)
        : Map_object(start_time_, duration_time_, x, y), position(position_) {}
};

//struct Conveyor_hold_note : Conveyor_note {
//private:
//    sf::Time duration;
//
//public:
//    Conveyor_hold_note(sf::Time &start_time_,
//                       sf::Time &duration_time_,
//                       float x,
//                       float y,
//                       Conveyor_note_key_position position_,
//                       sf::Time duration_)
//        : Conveyor_note(start_time_, duration_time_, x, y, position_),
//          duration(duration_) {}
//};

struct Bulletproof_shape : Map_object {
    //А что если создатель карты сам придумывает фигуру, можно сделать интерфейс
    //для создания фигур
};

}  // namespace USO
#endif  // USO_MAP_OBJECTS_H
