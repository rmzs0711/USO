//
// Created by Aigerim on 15.04.2021.
//

#include "menu.h"
#include <fstream>
#include <vector>
#include "maps.h"
#include "menu_objects.h"

namespace {
std::string list_of_saved_maps_file_name() {
    static std::string saved_maps = R"(data\saved_maps)";
    return saved_maps;
}
sf::Vector2f get_constructor_menu_buttons_size() {
    static sf::Vector2f sizes(500, 200);
    return sizes;
}
std::vector<std::string> get_vector_of_saved_maps_names() {
    std::vector<std::string> res;
    std::ifstream input(list_of_saved_maps_file_name());
    for (std::string line; !input.eof();) {
        std::getline(input, line);
        res.push_back(line);
    }
    return res;
}
}  // namespace

void Menu::start_menu() {
    sf::ContextSettings setting;
    setting.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(1080, 720), "USO!",
                            sf::Style::Fullscreen, setting);
    window.setVerticalSyncEnabled(true);
    window.display();

    std::vector<Menu::Button> buttons;
    buttons.emplace_back(400, 400, 200, Menu::OPEN_AIM);
    buttons.emplace_back(0, 0, 100, Menu::EXIT, sf::Color::Cyan);
    buttons.emplace_back(700, 50, 100, Menu::OPEN_SETTINGS, sf::Color::Green);
    buttons.emplace_back(900, 400, 200, Menu::OPEN_CONVEYOR,

                         sf::Color::Magenta);

    while (window.isOpen()) {
        window.clear();
        sf::Event event{};
        if (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::KeyPressed: {
                    if (event.key.code == sf::Keyboard::Escape) {
                        window.close();
                    }
                } break;
                case sf::Event::MouseButtonPressed: {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        for (auto &button : buttons) {

                            button.press(
                                window,
                                {static_cast<float>(event.mouseButton.x),
                                 static_cast<float>(event.mouseButton.y)});
                        }
                    }
                } break;
                case sf::Event::Closed: {
                    window.close();
                } break;
            }
        }
        for (auto &button : buttons) {
            button.guidance((sf::Vector2f)sf::Mouse::getPosition());
        }
        sf::CircleShape circle;
        circle.setRadius(20);
        circle.setPosition(
            sf::Vector2f(((sf::Vector2f)sf::Mouse::getPosition()).x - 20,
                         ((sf::Vector2f)sf::Mouse::getPosition()).y - 20));
        circle.setFillColor(sf::Color::Magenta);
        window.draw(circle);
        for (auto &button : buttons) {
            button.draw(window);
        }
        window.display();
    }
}

void Menu::constructor_menu(sf::Window &window) {
    std::vector<std::string> saved_maps(get_vector_of_saved_maps_names());
    sf::Vector2f sizes = get_constructor_menu_buttons_size();

    std::vector<Menu::Button> buttons;

    float start_x = static_cast<float>(window.getSize().x) - sizes.x;
    float start_y = sizes.y;
    for (auto &map_name: saved_maps) {
//        buttons.emplace_back(start_x, start_y, );
    }
}
