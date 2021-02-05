#ifndef USO_MAPS_H
#define USO_MAPS_H
#include <string>
#include <utility>
#include "SFML/Audio.hpp"
#include "SFML/Graphics.hpp"
#include "map_objects.h"

namespace USO {

struct Map {
private:
    std::string music_address;
    std::string music_name;
    std::string map_name;
    std::string author_name;
    std::vector<std::unique_ptr<Map_object>> map_objects;

public:
    // TODO setters, getters
    Map(std::string music_adress_,
        std::string music_name_,
        std::string map_name_,
        std::string author_name_,
        std::vector<std::unique_ptr<Map_object>> map_objects_ = {})
        : music_address(std::move(music_adress_)),
          music_name(std::move(music_name_)),
          map_name(std::move(map_name_)),
          author_name(std::move(author_name_)),
          map_objects(std::move(map_objects_)) {}
    virtual ~Map() = default;

    virtual void run(sf::RenderWindow &) = 0;
};

struct Aim_map final : Map {
    Aim_map(std::string music_adress_,
            std::string music_name_,
            std::string map_name_,
            std::string author_name_,
            std::vector<std::unique_ptr<Map_object>> map_objects_ = {})
        : Map(std::move(music_adress_),
              std::move(music_name_),
              std::move(map_name_),
              std::move(author_name_),
              std::move(map_objects_)) {}
    void run(sf::RenderWindow &) override;
};

struct Conveyor_map final : Map {
    void run(sf::RenderWindow &) override;
};

struct Bulletproof_map final : Map {
    void run(sf::RenderWindow &) override;
};

}  // namespace USO
#endif  // USO_MAPS_H
