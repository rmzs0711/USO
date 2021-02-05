#ifndef USO_MAP_MASTER_H
#define USO_MAP_MASTER_H

#include <list>
#include <memory>
#include <utility>
#include <vector>
#include "SFML/Graphics.hpp"
#include "base_logic.h"
#include "map_objects.h"
#include "maps.h"

namespace USO {

struct Map_choice_menu {
private:
    std::vector<std::unique_ptr<USO::Map>> map_pool;

public:
    explicit Map_choice_menu(std::vector<std::unique_ptr<USO::Map>> saved_maps)
        : map_pool(std::move(saved_maps)) {}
};

struct Field {
private:
    std::list<std::shared_ptr<Map_object>> field_objects;

public:
    void push(std::vector<std::shared_ptr<Map_object>>::iterator &map_object_it,
              sf::Time);
};

}  // namespace USO
#endif  // USO_MAP_MASTER_H
