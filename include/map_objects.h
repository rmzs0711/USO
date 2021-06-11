#ifndef USO_MAP_OBJECTS_H
#define USO_MAP_OBJECTS_H
#include <cmath>
#include <memory>
#include "SFML/Graphics.hpp"
#include "SFML/System/Time.hpp"
#include "base_logic.h"
namespace USO {

inline const int const_circle_beat_radius = 65;
inline const int const_active_circle_radius = 300;
inline const sf::Time const_active_circle_duration = sf::seconds(0.7);
inline const sf::Color purple(100, 5, 94);
inline const sf::Time const_active_note_duration = sf::seconds(2);
inline const sf::Vector2f const_line_pos = {500, 0};
inline const sf::Vector2f const_line_size = {200, 1100};

const double time_per_pixels = 2;

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
               const sf::Time &move_time_);

public:
    virtual bool change_state(const sf::Time &) = 0;
    virtual bool check_event(const sf::Vector2f &,
                             BL::Game_session &game_session,
                             const sf::Time &) = 0;
    virtual void draw(sf::RenderWindow &window, const sf::Font &font) = 0;
    virtual ~Map_object() = default;
    virtual sf::Time &get_start_time();
    virtual sf::Time &get_duration_time();
    [[nodiscard]] virtual const sf::Vector2f &get_pos() const;
    [[nodiscard]] virtual const sf::Vector2f &get_end_pos() const;
    [[nodiscard]] virtual sf::Vector2f *get_end_pos_ptr() = 0;
    virtual std::shared_ptr<Map_object> clone() = 0;
    virtual void reset();
    [[nodiscard]] virtual const sf::Time &get_move_time() const;
    void set_move_time(const sf::Time &);
};

struct Aim_circle : Map_object {
protected:
    float beat_radius;
    float active_circle_radius = 0;
    float active_circle_start_radius;
    bool is_valid = true;
    float time_coef = 1;

public:
    static bool is_circle_correct_click(const sf::Vector2f &mouse,
                                        const sf::Vector2f &center,
                                        const float &radius);
    Aim_circle(const sf::Time &start_time_,
               const sf::Time &duration_time_,
               float x,
               float y,
               float beat_radius_,
               float active_circle_radius_,
               const sf::Time &move_time_ = sf::seconds(0));

    Aim_circle(const Aim_circle &) = default;

    bool change_state(const sf::Time &current_time) override;
    bool check_event(const sf::Vector2f &,
                     BL::Game_session &game_session,
                     const sf::Time &current_time) override;
    void draw(sf::RenderWindow &window, const sf::Font &font) override;
    sf::Vector2f *get_end_pos_ptr() override;
    std::shared_ptr<Map_object> clone() override;
    void reset() override;
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
               const sf::Time &move_time_);
    bool change_state(const sf::Time &current_time) override;
    bool check_event(const sf::Vector2f &,
                     BL::Game_session &game_session,
                     const sf::Time &current_time) override;
    void draw(sf::RenderWindow &window, const sf::Font &font) override;
    [[nodiscard]] const sf::Vector2f &get_end_pos() const override;
    [[nodiscard]] sf::Vector2f *get_end_pos_ptr() override;
    std::shared_ptr<Map_object> clone() override;
    [[nodiscard]] const sf::Vector2f &get_start_pos() const;
    void reset() override;
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
                float active_circle_start_radius_);

    float calc_delta(const sf::Vector2f &, float &);
    bool change_state(const sf::Time &current_time) override;
    bool check_event(const sf::Vector2f &,
                     BL::Game_session &game_session,
                     const sf::Time &current_time) override;
    void draw(sf::RenderWindow &window, const sf::Font &font) override;
    static bool check_sum_of_radians(float &);
    bool check_event_for_draw(const sf::Vector2f &);
    std::shared_ptr<Map_object> clone() override;
};

struct Conveyor_line {
public:
    sf::Vector2f pos;
    sf::Vector2f sizes;
    sf::Vector2f beat_pos;
    sf::Vector2f beat_sizes;
    bool dragged = false;
    bool missed = false;
    const int index;
    Conveyor_line(sf::Vector2f pos_, sf::Vector2f sizes_, int index_);
    void draw(sf::RenderWindow &window) const;
};

struct Conveyor_note : Map_object {
private:
    Conveyor_line &line;

public:
    Conveyor_note(const sf::Time &start_time_,
                  const sf::Time &duration_time_,
                  Conveyor_line &line_);
    static bool is_note_correct_click(const sf::Vector2f &mouse_pos,
                                      const sf::Vector2f &pos,
                                      const USO::Conveyor_line &line_);
    bool change_state(const sf::Time &current_time) override;
    bool check_event(const sf::Vector2f &,
                     BL::Game_session &game_session,
                     const sf::Time &current_time) override;
    void draw(sf::RenderWindow &window, const sf::Font &font) override;
    std::shared_ptr<Map_object> clone() override;
    sf::Vector2f *get_end_pos_ptr() override;
    [[nodiscard]] const USO::Conveyor_line &get_line() const;
};

}  // namespace USO
#endif  // USO_MAP_OBJECTS_H
