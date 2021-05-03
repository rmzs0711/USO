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

namespace {}

namespace USO {
void Aim_map::constructor_run(sf::RenderWindow &window) {
    USO::Field field(window, {});
    auto current_object_it =
        map_objects.begin();  // итератор на следующий по времени объект

    sf::Time current_time;

    sf::Music music;
    sf::SoundBuffer sound_buf;
    sf::Texture image;
    sf::Font font;

    prelude(music, sound_buf, image, font);  // Загружает и проверяет

    std::vector<std::shared_ptr<Map_object>> editing_map_objects;
    editing_map_objects.reserve(map_objects.size());
    for (std::shared_ptr<Map_object> &ptr : map_objects) {
        editing_map_objects.emplace_back(ptr->clone());
    }

    sf::Sound sound;
    sound.setBuffer(sound_buf);

    sf::RectangleShape rect(static_cast<sf::Vector2f>(window.getSize()));
    rect.setPosition(0, 0);
    rect.setTexture(&image);

    std::vector<int> dragged_key(
        sf::Keyboard::KeyCount);  // чтобы различать какая кнопка зажата
    std::vector<int> dragged_mouse_button(
        sf::Mouse::ButtonCount);  // то же самое только про мышку

    bool drag = true;

    sf::Event event{};

    // WARNING: lambda zone
    auto handle_left_click = [&]() -> void {
        drag = true;  //Зажимаю мышку
        if (field.get_field_objects().empty()) {
            return;
        }
        USO::Map_object &front_object =
            *(field.get_field_objects().front().get());

        if (typeid(front_object) == typeid(USO::Aim_slider)) {
        }
        // TODO handle click
    };
    // lambda zone ends

    while (true) {
        window.draw(rect);
        if (current_object_it != map_objects.end()) {
            if (*current_object_it) {
                field.push(current_object_it, current_time);
            } else {
                std::cerr << "invalid object iterator" << std::endl;
                current_object_it++;
            }
        }

        field.change_state(current_time);

        if (!field.get_field_objects().empty() &&
            !field.get_field_objects().back()->change_state(current_time)) {
            field.get_field_objects().pop_back();
        }

        field.draw(font);
        window.display();

        if (!window.pollEvent(event) && !drag) {
            continue;
        }

        switch (event.type) {
            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Escape) {
                    // Покидаем
                    return;
                }
                if (event.key.code != sf::Keyboard::Z &&
                    event.key.code != sf::Keyboard::X) {
                    break;
                }
                if (event.key.code == sf::Keyboard::Z) {
                    dragged_key[event.key.code] = 1;
                    handle_left_click();
                } else {
                    handle_right_click();
                }
                break;
            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button != sf::Mouse::Left &&
                    event.mouseButton.button != sf::Mouse::Right) {
                    break;
                }
                if (event.mouseButton.button == sf::Mouse::Left) {
                    dragged_key[event.key.code] = 1;
                    handle_left_click();
                    dragged_mouse_button[event.mouseButton.button] = 1;
                } else {
                    handle_right_click();
                }
                break;
            case sf::Event::KeyReleased:
                if (!drag) {
                    break;
                }
                if (event.key.code != sf::Keyboard::Z &&
                    event.key.code != sf::Keyboard::X) {
                    break;
                }
                if (dragged_key[event.key.code] == 0) {
                    break;
                }
                dragged_key[event.key.code] = 0;
                drag = false;
                break;
            case sf::Event::MouseButtonReleased:
                if (!drag) {
                    break;
                }
                if (dragged_mouse_button[event.mouseButton.button] == 0) {
                    break;
                }
                dragged_mouse_button[event.mouseButton.button] = 0;
                drag = false;
                break;
            default:
                if (drag && !field.get_field_objects().empty()) {
                    USO::Map_object &front_object =
                        *(field.get_field_objects().front().get());
                    if (typeid(front_object) != typeid(USO::Aim_slider)) {
                        break;
                    }
                    // todo
                }
                break;
        }
        break;
    }
}
}  // namespace USO