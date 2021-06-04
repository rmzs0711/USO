#ifndef USO_MAPS_H
#define USO_MAPS_H
#include <iostream>
#include <memory>
#include <string>
#include <list>
#include <utility>
#include "SFML/Audio.hpp"
#include "SFML/Graphics.hpp"
#include "map_objects.h"
#include "base_logic.h"

namespace USO {

struct Map {
protected:
    std::string mode;
    std::string map_name;
    std::string map_address;
    std::string music_address;
    std::string music_name;
    std::string image_address;
    std::string font_address;
    std::string sound_address;
    std::list<std::shared_ptr<Map_object>> map_objects;

    static void check_file_load(const bool &condition,
                                const std::string &filename) {
        if (!condition) {
            // Неудачная загрузка файла
            std::cerr << "where's the " + filename + " Lebovski" << std::endl;
        }
    }

public:
    // TODO setters, getters
    Map(std::string mode_,
        std::string map_name_,
        std::string map_address_,
        std::string music_address_,
        std::string music_name_,
        std::string image_address_,
        std::string font_address_,
        std::string sound_address_,
        std::list<std::shared_ptr<Map_object>> map_objects_ = {})
        : mode(std::move(mode_)),
          map_name(std::move(map_name_)),
          map_address(std::move(map_address_)),
          music_address(std::move(music_address_)),
          music_name(std::move(music_name_)),
          image_address(std::move(image_address_)),
          font_address(std::move(font_address_)),
          sound_address(std::move(sound_address_)),
          map_objects(std::move(map_objects_)) {}

    virtual ~Map() = default;
    void prelude(sf::Music &music,
                 sf::SoundBuffer &sound_buffer,
                 sf::Texture &,
                 sf::Font &) const;

    std::string get_map_name();
    std::string get_music_name();

    virtual void run(sf::RenderWindow &) = 0;
    virtual void constructor_run(sf::RenderWindow &){};

protected:
    Map() = default;
};

struct Aim_map final : Map {
    Aim_map() = default;
    Aim_map(std::string mode_,
            std::string map_name_,
            std::string map_address_,
            std::string music_address_,
            std::string music_name_,
            std::string image_address_,
            std::string font_address_,
            std::string sound_address_,
            std::list<std::shared_ptr<Map_object>> map_objects_ = {})
        : Map(std::move(mode_),
              std::move(map_name_),
              std::move(map_address_),
              std::move(music_address_),
              std::move(music_name_),
              std::move(image_address_),
              std::move(font_address_),
              std::move(sound_address_),
              std::move(map_objects_)) {}

    explicit Aim_map(const std::string &filename);

    void run(sf::RenderWindow &) override;
    void constructor_run(sf::RenderWindow &) override;
};

struct Conveyor_map final : Map {
private:
    const int NUMBER_OF_LINES = 4;
    std::vector<std::shared_ptr<Conveyor_line>> lines;
    void generate_lines() {
        lines.push_back(std::make_shared<USO::Conveyor_line>(USO::Conveyor_line(
            const_line_pos, const_line_size, 0)));

        for (int i = 1; i < NUMBER_OF_LINES; ++i) {
            lines.push_back(std::make_shared<USO::Conveyor_line>(
                USO::Conveyor_line(sf::Vector2f(const_line_pos.x + (float)i * const_line_size.x, const_line_pos.y),
                                   const_line_size, i)));
        }
    }
public:
    Conveyor_map(std::string mode_,
    std::string map_name_,
        std::string map_adress,
    std::string music_address_,
        std::string music_name_,
    std::string image_address_,
        std::string font_address_,
    std::string sound_address_,
        std::list<std::shared_ptr<Map_object>> map_objects_ = {})
    : Map(std::move(mode_),
        std::move(map_name_),
        std::move(map_adress),
        std::move(music_address_),
        std::move(music_name_),
        std::move(image_address_),
        std::move(font_address_),
        std::move(sound_address_),
        std::move(map_objects_)) {
        generate_lines();
    }

    explicit Conveyor_map(const std::string &filename);

    void run(sf::RenderWindow &) override;
    void constructor_run(sf::RenderWindow &) override;
};

struct Bulletproof_map final : Map {
    void run(sf::RenderWindow &) override;
};

}  // namespace USO
#endif  // USO_MAPS_H
