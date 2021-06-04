#ifndef USO_MENU_H
#define USO_MENU_H
#include <cstddef>
#include <deque>
#include <string>
#include "SFML/Graphics.hpp"
#include "base_logic.h"

namespace Menu {

const std::size_t MAX_SIZE = 6;

struct scrolling_menu {
private:
    sf::Text text;
    sf::Font font;
    int delta{};
    std::vector<std::string> list_of_maps;
    std::vector<sf::RectangleShape> blocks_of_maps_name;
    std::vector<sf::RectangleShape> blocks_of_maps_data;
    std::vector<std::string> list_of_data;
    std::string filename;
    sf::RectangleShape construct_new_map;

public:
    scrolling_menu() = default;
    explicit scrolling_menu(std::string);
    void scrolling_down();
    void scrolling_up();
    bool push(sf::RenderWindow &, sf::Vector2f);
    void draw(sf::RenderWindow &window);

    void add_new_map(const std::string &);
    [[nodiscard]] int get_delta() const;
    void increase_delta();
    void decrease_delta();
    void get_input(sf::RenderWindow &);
    int get_id(sf::Vector2f) const;
};

// struct data {
// private:
//
//    std::string mode;
//    std::string map_name;
//    std::string author_name;
//    std::string music_address;
//    std::string music_name;
//    std::string image_address;
//    std::string font_address;
//    std::string sound_address;
// public:
//
//    data(std::string, std::string, std::string, std::string,
//         std::string, std::string, std::string, std::string);
//    void input();
//};

void menu(sf::RenderWindow &, BL::Game_session);
void constructor_menu(sf::Window &window);
sf::RenderWindow &set_settings();
void stop_menu(sf::RenderWindow &, BL::Game_session &);
void list_of_maps(sf::RenderWindow &);
}  // namespace Menu
#endif  // USO_MENU_H
