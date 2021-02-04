#ifndef USO_MAPS_H
#define USO_MAPS_H
#include <string>
#include "SFML/Audio.hpp"
#include "SFML/Graphics.hpp"

namespace USO {

struct Map {
    std::string music_address;
    std::string music_name;
    std::string map_name;
    std::string author_name;
    virtual void run(sf::RenderWindow&) = 0;
};

struct Aim_map : Map {
    void run(sf::RenderWindow &) override;
};

struct Conveyor_map : Map {
    void run(sf::RenderWindow &) override;
};

struct Bulletproof_map : Map {
    void run(sf::RenderWindow &) override;
};

}  // namespace USO
#endif  // USO_MAPS_H
