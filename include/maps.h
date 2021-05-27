#ifndef USO_MAPS_H
#define USO_MAPS_H
#include <memory>
#include <string>
#include <iostream>
#include <utility>
#include "SFML/Audio.hpp"
#include "SFML/Graphics.hpp"
#include "map_objects.h"

namespace USO {

struct Map {
public:
    std::string music_address;
    std::string music_name;
    std::string map_name;
    std::string author_name;
    std::vector<std::shared_ptr<Map_object>> map_objects;

    static void check_file_load(const bool &condition, const std::string &filename) {
        if (!condition) {
            // Неудачная загрузка файла
            std::cerr << "where's the " + filename + " Lebovski" << std::endl;
        }
    }

public:
    // TODO setters, getters
    Map(std::string music_adress_,
        std::string music_name_,
        std::string map_name_,
        std::string author_name_,
        std::vector<std::shared_ptr<Map_object>> map_objects_ = {})
        : music_address(std::move(music_adress_)),
          music_name(std::move(music_name_)),
          map_name(std::move(map_name_)),
          author_name(std::move(author_name_)),
          map_objects(std::move(map_objects_)) {}
    virtual ~Map() = default;

    virtual void run(sf::RenderWindow &) = 0;
    // virtual void construct(sf::RenderWindow &) = 0;

protected:
    Map() = default;
};

struct Aim_map final : Map {
    explicit Aim_map(const std::string &filename);

    void run(sf::RenderWindow &) override;
//    void construct(sf::RenderWindow &) override;
};

struct Conveyor_map final : Map {
private:
    const int NUMBER_OF_LINES = 4;
public:
    explicit Conveyor_map(const std::string &filename);
    std::vector<std::shared_ptr<Conveyor_line>> lines;

    void run(sf::RenderWindow &) override;
    // void construct(sf::RenderWindow &);
};

struct Bulletproof_map final : Map {
    void run(sf::RenderWindow &) override;
};

}  // namespace USO
#endif  // USO_MAPS_H
