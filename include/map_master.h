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
    explicit Map_choice_menu(std::string& saved_maps_names_list)
        : map_pool({}) {
        //TODO читается файл по заданному адресу и потом по адресам ищет карты
    }
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
