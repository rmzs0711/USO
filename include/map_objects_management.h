#ifndef USO_MAP_OBJECTS_MANAGEMENT_H
#define USO_MAP_OBJECTS_MANAGEMENT_H

#include <list>
#include <memory>
//#include <utility>
//#include <vector>
#include "SFML/Graphics.hpp"
//#include "base_logic.h"
#include "map_objects.h"
//#include "maps.h"

namespace USO {

struct Field {
private:
    sf::RenderWindow &window;
    std::list<std::shared_ptr<Map_object>> field_objects;

public:
    Field(sf::RenderWindow &window_,
          std::list<std::shared_ptr<Map_object>> field_objects_)
        : window(window_), field_objects(std::move(field_objects_)) {}
    void draw(const sf::Font &);
    void change_state(const sf::Time &);
    void push_front(
        std::list<std::shared_ptr<Map_object>>::iterator &map_object_it,
        sf::Time);
    std::list<std::shared_ptr<Map_object>> &get_field_objects();
};
}  // namespace USO

#endif  // USO_MAP_OBJECTS_MANAGEMENT_H
