#ifndef USO_MAP_H
#define USO_MAP_H
#include <string>
#include "SFML/Audio.hpp"
#include "SFML/Graphics.hpp"

namespace USO {

struct Map {
    std::string music_address;
    std::string music_name;
    std::string map_name;
    std::string author_name;
};

struct Aim_map : Map {};

struct Conveyor_map : Map {};

struct Bulletproof_map : Map {};

struct Map_object_circle {
private:
    sf::Time start_time;
    sf::Time duration_time;
    float beat_radius;
    float active_circle_radius;

public:
    Map_object_circle(sf::Time &start_time_,
                      sf::Time &duration_time_,
                      float &beat_radius_,
                      float &active_circle_radius_)
        : start_time(start_time_),
          duration_time(duration_time_),
          beat_radius(beat_radius_),
          active_circle_radius(active_circle_radius_) {}
};

}  // namespace USO
#endif  // USO_MAP_H
