//
// Created by Aigerim on 15.04.2021.
//

#include "menu.h"
#include <vector>
#include "menu_objects.h"

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
