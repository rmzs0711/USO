#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>
#include <typeindex>
#include "SFML/Graphics.hpp"
#include "base_logic.h"
#include "iterator"
#include "map_management.h"
#include "maps.h"

namespace USO {
void Aim_map::construct(sf::RenderWindow &window) {
    USO::Field field(window, {});
    sf::Time current_time;
    auto current_object_it =
        map_objects.begin();  // итератор на следующий по времени объект
    sf::Music music;
    music.openFromFile(music_address);

    std::vector<std::shared_ptr<Map_object>> editing_map_objects;

    editing_map_objects.reserve(map_objects.size());
//    for (std::shared_ptr<Map_object> & ptr : map_objects) {
//
//        editing_map_objects.emplace_back(std::make_shared<Aim_circle>(*ptr));
//    }
//    for (std::shared_ptr<Map_object> &ptr : map_objects) {
//        ;
//    }
}
}  // namespace USO