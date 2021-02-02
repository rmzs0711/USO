#ifndef USO_MAP_CHOICE_H
#define USO_MAP_CHOICE_H

#include <memory>
#include <utility>
#include <vector>
#include "SFML/Graphics.hpp"
#include "base_logic.h"
#include "map.h"

namespace USO {

struct Map_choice_menu {
private:
    std::vector<std::unique_ptr<USO::Map>> map_pool;

public:
    explicit Map_choice_menu(std::vector<std::unique_ptr<USO::Map>> saved_maps)
        : map_pool(std::move(saved_maps)) {}

    void run_map(std::unique_ptr<USO::Map>, sf::RenderWindow &);
};

struct Field {
    std::vector<Map_object_circle> circles;
};
}  // namespace USO
#endif  // USO_MAP_CHOICE_H
