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

void draw_input_text_box(sf::RenderWindow &window,
                         sf::Time &time_delta,
                         const sf::Font &font) {
    sf::Event event{};
    sf::RectangleShape input_field(sf::Vector2f(500, 100));
    input_field.setFillColor(sf::Color::White);
    input_field.setPosition(sf::Vector2f(window.getSize()) / 2.0f -
                            sf::Vector2f(input_field.getSize()) / 2.0f);
    sf::Int64 input_in_int = 0;
    std::string input;
    sf::Text genius_text;
    genius_text.setPosition(input_field.getPosition());
    genius_text.setFont(font);
    genius_text.setCharacterSize(80);
    genius_text.setFillColor(sf::Color::Red);
    while (true) {
        if (!window.pollEvent(event)) {
            continue;
        }
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                return;
            } else if (event.key.code == sf::Keyboard::Enter) {
                time_delta = sf::microseconds(input_in_int);
                break;
            } else if (event.key.code == sf::Keyboard::BackSpace) {
                input_in_int /= 10;
                if (!input.empty()) {
                    input.pop_back();
                }
                genius_text.setString(input);
            }
        } else if (event.type == sf::Event::TextEntered) {
            if (input.size() < 10 &&
                std::isdigit(static_cast<char>(event.text.unicode))) {
                input_in_int *= 10;
                input_in_int += static_cast<char>(event.text.unicode) - '0';
                input += static_cast<char>(event.text.unicode);
                genius_text.setString(input);
            }
        }
        window.draw(input_field);
        window.draw(genius_text);
        window.display();
    }
}

}  // namespace

namespace USO {
void Aim_map::constructor_run(sf::RenderWindow &window) {
    //    USO::Field field(window, {});
    std::fstream fout;
    fout.open(R"(data/maps/editing_map.txt)", std::ios::out | std::ios::trunc);
    if (!fout) {
        return;
    }
    std::list<std::shared_ptr<Map_object>> editing_map_objects;
    for (std::shared_ptr<Map_object> &ptr : map_objects) {
        editing_map_objects.emplace_back(ptr->clone());
    }
    auto end_draw_iterator =
        editing_map_objects.begin();  // итератор на следующий по времени объект
    auto start_draw_iterator = editing_map_objects.begin();

    std::shared_ptr<Map_object> dragged_object;

    sf::Time current_time;
    sf::Time time_delta = sf::microseconds(1e6);
    sf::Clock clock;
    sf::Time remembered_time;

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

        for (auto i = start_draw_iterator; i != end_draw_iterator; i++) {
            //            if (typeid(i) == typeid(USO::Aim_slider)) {
            //                if (is_circle_correct_click(
            //                        sf::Vector2f(sf::Mouse::getPosition(window)),
            //                        (*i)->get_end_pos(),
            //                        const_circle_beat_radius)) {
            //                    dragged_object = *i;
            //                    drag_end_pos = true;
            //                }
            //            }

            if (is_circle_correct_click(
                    sf::Vector2f(sf::Mouse::getPosition(window)),
                    (*i)->get_pos(), const_circle_beat_radius)) {
                dragged_object = *i;
                return;
            }
        }
        switch (object_to_create) {
            case OBJECT_TO_CREATE::CIRCLE:
                editing_map_objects.insert(
                    start_draw_iterator,
                    std::make_shared<Aim_circle>(Aim_circle(
                        current_time - active_circle_duration,
                        active_circle_duration,
                        (float)sf::Mouse::getPosition(window).x,
                        (float)sf::Mouse::getPosition(window).y,
                        const_circle_beat_radius, const_active_circle_radius)));
                start_draw_iterator--;
                break;
            case OBJECT_TO_CREATE::SLIDER:

                break;
            case OBJECT_TO_CREATE::SPINNER:
                break;
        }
        dragged_object = *start_draw_iterator;
    };

    auto handle_right_click = [&]() {
        for (auto i = start_draw_iterator; i != end_draw_iterator; i++) {
            if (is_circle_correct_click(
                    sf::Vector2f(sf::Mouse::getPosition(window)),
                    (*i)->get_pos(), const_circle_beat_radius)) {
                if (i == start_draw_iterator) {
                    start_draw_iterator++;
                }
                editing_map_objects.erase(i);
                return;
            }
        }
    };

    auto inc_time = [&]() {
        current_time += time_delta;
        if (editing_map_objects.empty()) {
            return;
        }

        while (start_draw_iterator != editing_map_objects.end() &&
               (*start_draw_iterator)->get_start_time() +
                       (*start_draw_iterator)->get_duration_time() <
                   current_time) {
            (*start_draw_iterator)->reset();
            start_draw_iterator++;
        }
        while (end_draw_iterator != editing_map_objects.end() &&
               (*end_draw_iterator)->get_start_time() <= current_time) {
            (*end_draw_iterator)->reset();
            end_draw_iterator++;
        }
    };

    auto dec_time = [&]() {
        current_time -= time_delta;
        if (current_time < sf::seconds(0)) {
            current_time = sf::seconds(0);
        }
        if (editing_map_objects.empty()) {
            return;
        }

        if (start_draw_iterator == editing_map_objects.end()) {
            start_draw_iterator--;
        }
        while (start_draw_iterator != editing_map_objects.begin() &&
               (*start_draw_iterator)->get_start_time() +
                       (*start_draw_iterator)->get_duration_time() >
                   current_time) {
            (*start_draw_iterator)->reset();
            start_draw_iterator--;
        }
        if ((*start_draw_iterator)->get_start_time() +
                (*start_draw_iterator)->get_duration_time() <
            current_time) {
            (*start_draw_iterator)->reset();
            start_draw_iterator++;
        }

        if (end_draw_iterator == editing_map_objects.end()) {
            end_draw_iterator--;
        }

        while (end_draw_iterator != editing_map_objects.begin() &&
               (*end_draw_iterator)->get_start_time() > current_time) {
            (*end_draw_iterator)->reset();
            end_draw_iterator--;
            // TODO
        }
        (*end_draw_iterator)->reset();
        end_draw_iterator++;
    };
    // lambda zone ends

    while (true) {
        if (music.getStatus() == sf::Music::Playing) {
            current_time = clock.getElapsedTime() + remembered_time;
            inc_time();
        }
        std::cout << current_time.asSeconds() << std::endl;
        for (auto i = editing_map_objects.begin();
             i != editing_map_objects.end(); i++) {
            if (i == end_draw_iterator) {
                std::cout << "*";
            } else {
                std::cout << "-";
            }
        }
        std::cout << std::endl;
        window.draw(rect);
        //        if (end_draw_iterator != editing_map_objects.end()) {
        //            if (*end_draw_iterator) {
        //                field.push_front(end_draw_iterator, current_time);
        //            } else {
        //                std::cerr << "invalid object iterator" <<
        //                std::endl;
        //            }
        //        }

        //        field.change_state(current_time);

        //        field.draw(font);

        for (auto i = start_draw_iterator; i != end_draw_iterator; i++) {
            if ((*i)->get_start_time() > current_time) {
                continue;
            }
            (*i)->change_state(current_time);
            (*i)->draw(window, font);
        }

        window.display();

        if (!window.pollEvent(event) && !drag) {
            continue;
        }

        switch (event.type) {
            case sf::Event::KeyPressed:
                if (dragged_object && drag) {
                    //                    auto &object = *dragged_object;
                    //                    if (typeid(object) ==
                    //                    typeid(USO::Aim_slider)) {
                    //                        if (drag_end_pos) {
                    //                            dragged_object->get_end_pos()
                    //                            =
                    //                                sf::Vector2f(sf::Mouse::getPosition(window));
                    //                            continue;
                    //                        }
                    //                    }
                    dragged_object->get_pos() =
                        sf::Vector2f(sf::Mouse::getPosition(window));
                    break;
                }
                if (event.key.code == sf::Keyboard::Escape) {
                    fout << map_name << std::endl;
                    fout << author_name << std::endl;
                    fout << music_address << std::endl;
                    fout << music_name << std::endl;
                    fout << image_address << std::endl;
                    fout << font_address << std::endl;
                    fout << sound_address << std::endl;
                    for (auto &i : editing_map_objects) {
                        fout << "Aim_circle" << std::endl;
                        fout << i->get_start_time().asMicroseconds() << std::endl;
                        fout << i->get_duration_time().asMicroseconds() << std::endl;
                        fout << i->get_pos().x << std::endl;
                        fout << i->get_pos().y << std::endl;
                        fout << const_circle_beat_radius << " " << const_active_circle_radius << std::endl;
                    }
                    fout.close();
                    return;
                }
                if (event.key.code == sf::Keyboard::Space) {
                    remembered_time = current_time;
                    time_delta = sf::seconds(0);
                    clock.restart();
                    music.setPlayingOffset(current_time);
                    music.play();
                    break;
                }
                if (event.key.code == sf::Keyboard::Right) {
                    inc_time();
                } else if (event.key.code == sf::Keyboard::Left) {
                    dec_time();
                }
                if (event.key.code == sf::Keyboard::Z) {
                    dragged_key[event.key.code] = 1;
                    handle_left_click();
                } else if (event.key.code == sf::Keyboard::X) {
                    handle_right_click();
                } else if (event.key.code == sf::Keyboard::D) {
                    draw_input_text_box(window, time_delta, font);
                }
                break;
            case sf::Event::MouseMoved:
                if (dragged_object && drag) {
                    dragged_object->get_pos() =
                        sf::Vector2f(sf::Mouse::getPosition(window));
                    break;
                }
            case sf::Event::MouseButtonPressed:
                if (dragged_object && drag) {
                    //                    auto &object = *dragged_object;
                    //                    if (typeid(object) ==
                    //                    typeid(USO::Aim_slider)) {
                    //                        if (drag_end_pos) {
                    //                            dragged_object->get_end_pos()
                    //                            =
                    //                                sf::Vector2f(sf::Mouse::getPosition(window));
                    //                            continue;
                    //                        }
                    //                    }
                    dragged_object->get_pos() =
                        sf::Vector2f(sf::Mouse::getPosition(window));
                    break;
                }
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
                    inc_time();
                } else {
                    dec_time();
                }
                break;
            default:
                //                if (drag &&
                //                !field.get_field_objects().empty()) {
                //                    USO::Map_object &front_object =
                //                        *(field.get_field_objects().front().get());
                //                    if (typeid(front_object) !=
                //                    typeid(USO::Aim_slider)) {
                //                        break;
                //                    }
                //                }
                break;
        }
    }
}
}  // namespace USO