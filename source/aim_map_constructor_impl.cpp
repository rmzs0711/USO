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

namespace {
enum class OBJECT_TO_CREATE { CIRCLE, SLIDER, SPINNER };
bool is_circle_correct_click(const sf::Vector2f &mouse,
                             const sf::Vector2f &center,
                             const float &radius) {
    return (mouse.x - center.x) * (mouse.x - center.x) +
               (mouse.y - center.y) * (mouse.y - center.y) <=
           radius * radius;
}
}  // namespace

namespace USO {
void Aim_map::constructor_run(sf::RenderWindow &window) {
    USO::Field field(window, {});

    std::list<std::shared_ptr<Map_object>> editing_map_objects;
    for (std::shared_ptr<Map_object> &ptr : map_objects) {
        editing_map_objects.emplace_back(ptr->clone());
    }
    auto current_object_it =
        editing_map_objects.begin();  // итератор на следующий по времени объект

    std::shared_ptr<Map_object> dragged_object;

    sf::Time current_time = sf::microseconds(1);
    sf::Time time_delta;
    sf::Music music;
    sf::SoundBuffer sound_buf;
    sf::Texture image;
    sf::Font font;

    prelude(music, sound_buf, image, font);  // Загружает и проверяет

    OBJECT_TO_CREATE object_to_create = OBJECT_TO_CREATE::CIRCLE;

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
    bool drag_end_pos = false;
    sf::Event event{};

    // WARNING: lambda zone
    auto handle_left_click = [&]() -> void {
        drag = true;
        for (auto &i : field.get_field_objects()) {
            if (typeid(i) == typeid(USO::Aim_slider)) {
                if (is_circle_correct_click(
                        sf::Vector2f(sf::Mouse::getPosition(window)),
                        i->get_end_pos(), circle_beat_radius)) {
                    dragged_object = i;
                    drag_end_pos = true;
                }
            }
            if (is_circle_correct_click(
                    sf::Vector2f(sf::Mouse::getPosition(window)), i->get_pos(),
                    circle_beat_radius)) {
                dragged_object = i;
                return;
            }
        }
//        switch (object_to_create) {
//            case OBJECT_TO_CREATE::CIRCLE:
//                editing_map_objects.insert(
//                    current_object_it,
//                    std::make_shared<Aim_circle>(
//                        Aim_circle(current_time - active_circle_duration,
//                                   active_circle_duration, ,
//                                   sf::Mouse::getPosition(window).x,
//                                   sf::Mouse::getPosition(window).y, circle_beat_radius, active_circle_radius)));
//                break;
//            case OBJECT_TO_CREATE::SLIDER:
//                break;
//            case OBJECT_TO_CREATE::SPINNER:
//                break;
//        }
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
                if (dragged_object && drag) {
                    auto &object = *dragged_object;
                    if (typeid(object) == typeid(USO::Aim_slider)) {
                        if (drag_end_pos) {
                            dragged_object->get_end_pos() =
                                sf::Vector2f(sf::Mouse::getPosition(window));
                            continue;
                        }
                    }
                    dragged_object->get_pos() =
                        sf::Vector2f(sf::Mouse::getPosition(window));
                }
                if (event.key.code == sf::Keyboard::Escape) {
                    //                     Покидаем
                    return;
                }
                if (event.key.code == sf::Keyboard::Z) {
                    dragged_key[event.key.code] = 1;
                    handle_left_click();
                } else if (event.key.code == sf::Keyboard::X) {
                    // handle_right_click();
                } else if (event.key.code == sf::Keyboard::D) {
                    sf::RectangleShape input_field(sf::Vector2f(500, 100));
                    input_field.setFillColor(sf::Color::White);
                    input_field.setPosition(
                        sf::Vector2f((float)window.getSize().x / 2 - 250,
                                     (float)window.getSize().y / 2 - 50));
                    sf::Uint32 genius_input = 0;
                    sf::Text genius_text;
                    while (true) {
                        if (event.type == sf::Event::KeyPressed) {
                            if (event.key.code == sf::Keyboard::Escape) {
                                break;
                            } else if (event.key.code == sf::Keyboard::Enter) {
                                time_delta = sf::microseconds(genius_input);
                                break;
                            } else if (event.key.code ==
                                       sf::Keyboard::BackSpace) {
                                genius_input /= 10;
                                genius_text.setString(genius_input);
                            }
                        } else if (event.type == sf::Event::TextEntered) {
                            if (std::isdigit(
                                    static_cast<char>(event.text.unicode))) {
                                genius_input *= 10;
                                genius_input +=
                                    static_cast<char>(event.text.unicode) - '0';
                                genius_text.setString(genius_input);
                            }
                        }
                    }
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
                    //                    handle_right_click();
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
                drag_end_pos = false;
                dragged_object.reset();
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
            case sf::Event::MouseWheelMoved:
                if (event.mouseWheel.delta > 0) {
                    current_time += time_delta;
                } else {
                    current_time -= time_delta;
                }
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