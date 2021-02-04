#ifndef USO_MAP_MASTER_H
#define USO_MAP_MASTER_H

#include <memory>
#include <utility>
#include <vector>
#include "SFML/Graphics.hpp"
#include "maps.h"
#include "base_logic.h"
#include "map_objects.h"

namespace USO {

struct Map_choice_menu {
private:
    std::vector<std::unique_ptr<USO::Map>> map_pool;

public:
    explicit Map_choice_menu(std::vector<std::unique_ptr<USO::Map>> saved_maps)
        : map_pool(std::move(saved_maps)) {}
};

struct Field {
    std::vector<std::unique_ptr<Map_object>> circles;
};
}  // namespace USO
#endif  // USO_MAP_MASTER_H
