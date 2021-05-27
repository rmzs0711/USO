#ifndef USO_MAP_MANAGEMENT_H
#define USO_MAP_MANAGEMENT_H

#include <list>
#include <memory>
#include <utility>
#include <vector>
#include "SFML/Graphics.hpp"
#include "base_logic.h"
#include "map_objects.h"
#include "maps.h"

namespace USO {
struct Map_menu {
private:
    std::vector<std::unique_ptr<USO::Map>> map_pool;

public:
    explicit Map_menu(std::string &saved_maps_names_list) : map_pool({}) {
        // TODO читается файл по заданному адресу и потом по адресам ищет карты
    }
};
struct Map_choice_menu : Map_menu {
public:
    explicit Map_choice_menu(std::string &saved_maps_names_list)
        : Map_menu(saved_maps_names_list) {
        // TODO читается файл по заданному адресу и потом по адресам ищет карты
    }
    void run_map_choice_menu();
};

struct Map_constructor_menu : Map_menu {
private:
    std::vector<std::unique_ptr<USO::Map>> map_pool;

public:
    explicit Map_constructor_menu(std::string &saved_maps_names_list)
        : Map_menu(saved_maps_names_list) {
        // TODO читается файл по заданному адресу и потом по адресам ищет карты
    }
    void run_map_constructor_menu();
};

struct Field {
private:
    sf::RenderWindow &window;  // ATTENTION , в филде теперь храним ссылку на
                               // виндоу, чтобы он мог себя нарисовать
    std::list<std::shared_ptr<Map_object>> field_objects;

public:
    Field(sf::RenderWindow &window_,
          std::list<std::shared_ptr<Map_object>> field_objects_)
        : window(window_), field_objects(std::move(field_objects_)) {}
    void draw(const sf::Font &);
    void change_state(const sf::Time &);
    void push_front(std::list<std::shared_ptr<Map_object>>::iterator &map_object_it,
              sf::Time);
    void push_back(std::list<std::shared_ptr<Map_object>>::iterator &map_object_it);
    std::list<std::shared_ptr<Map_object>> &get_field_objects();
};
}  // namespace USO

#endif  // USO_MAP_MANAGEMENT_H
