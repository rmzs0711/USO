#include <cassert>
#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include "SFML/Graphics.hpp"
#include "base_logic.h"
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

double get_dist(sf::Vector2f start_pos, sf::Vector2f end_pos) {
    return std::pow((start_pos.x - end_pos.x) * (start_pos.x - end_pos.x) +
                        (start_pos.y - end_pos.y) * (start_pos.y - end_pos.y),
                    0.5);
}





}  // namespace

namespace USO {
void Aim_map::constructor_run(sf::RenderWindow &window) {
    sf::Color purple(100, 5, 94);
    std::list<std::shared_ptr<Map_object>> editing_map_objects;
    for (std::shared_ptr<Map_object> &ptr : map_objects) {
        editing_map_objects.emplace_back(ptr->clone());
    }
    auto end_draw_iterator =
        editing_map_objects.begin();  // итератор на следующий по времени объект
    auto start_draw_iterator = editing_map_objects.begin();

    sf::Time road_start_time;
    const int number_of_delta = 10;
    sf::RectangleShape road_rect(
        sf::Vector2f((float)window.getSize().x / 2, 80));
    road_rect.setPosition((float)window.getSize().x / 2,
                          (float)window.getSize().y - road_rect.getSize().y);
    road_rect.setFillColor(sf::Color(10, 145, 9, 50));
    road_rect.setOutlineThickness(2);
    road_rect.setOutlineColor(purple);

    sf::Vector2f *dragged_pos_ptr = nullptr;

    sf::Time current_time;
    sf::Time time_delta = sf::microseconds(1e6);
    sf::Time saved_delta = time_delta;
    sf::Clock clock;
    sf::Time remembered_time;
    sf::Time break_point_time;

    sf::Music music;
    sf::SoundBuffer sound_buf;
    sf::Texture image;
    sf::Font font;
    sf::Text current_time_text;
    current_time_text.setCharacterSize(80);
    current_time_text.setPosition(0, 0);
    current_time_text.setFont(font);
    current_time_text.setOutlineThickness(5);
    current_time_text.setOutlineColor(purple);
    current_time_text.setFillColor(sf::Color::White);

    sf::Text delta_text;
    delta_text.setCharacterSize(80);
    delta_text.setPosition(
        0, sf::Vector2f(window.getSize()).y -
               static_cast<float>(delta_text.getCharacterSize()));
    delta_text.setFont(font);
    delta_text.setFillColor(sf::Color::White);
    delta_text.setOutlineThickness(5);
    delta_text.setLetterSpacing(3);
    delta_text.setOutlineColor(purple);

    prelude(music, sound_buf, image, font);  // Загружает и проверяет

    OBJECT_TO_CREATE object_to_create = OBJECT_TO_CREATE::CIRCLE;

    sf::Sound sound;
    sound.setBuffer(sound_buf);

    sf::RectangleShape rect(sf::Vector2f(window.getSize()));
    rect.setPosition(0, 0);
    rect.setTexture(&image);

    std::vector<int> dragged_key(
        sf::Keyboard::KeyCount);  // чтобы различать какая кнопка зажата
    std::vector<int> dragged_mouse_button(
        sf::Mouse::ButtonCount);  // то же самое только про мышку

    bool slider_choosing_end = false;
    bool drag = true;
    sf::Event event{};

    // WARNING: lambda zone
    auto handle_left_click = [&]() -> void {
        if (drag) {
            return;
        }
        drag = true;

        music.pause();
        for (auto i = start_draw_iterator; i != end_draw_iterator; i++) {
            auto &object = **i;
            if (typeid(object) == typeid(Aim_circle)) {
                if (is_circle_correct_click(
                        sf::Vector2f(sf::Mouse::getPosition(window)),
                        (*i)->get_pos(), const_circle_beat_radius)) {
                    dragged_pos_ptr = &(*i)->get_pos();
                    return;
                }
            } else if (typeid(object) == typeid(Aim_slider)) {
                if (is_circle_correct_click(
                        sf::Vector2f(sf::Mouse::getPosition(window)),
                        (*i)->get_end_pos(), const_circle_beat_radius)) {
                    dragged_pos_ptr = &(*i)->get_end_pos();
                    return;
                }
            }
        }
        switch (object_to_create) {
            case OBJECT_TO_CREATE::CIRCLE:
                editing_map_objects.insert(
                    start_draw_iterator,
                    std::make_shared<Aim_circle>(Aim_circle(
                        current_time - const_active_circle_duration,
                        const_active_circle_duration,
                        (float)sf::Mouse::getPosition(window).x,
                        (float)sf::Mouse::getPosition(window).y,
                        const_circle_beat_radius, const_active_circle_radius)));
                start_draw_iterator--;
                dragged_pos_ptr = &(*start_draw_iterator)->get_pos();
                break;
            case OBJECT_TO_CREATE::SLIDER:

                editing_map_objects.insert(
                    start_draw_iterator,
                    std::make_shared<Aim_slider>(Aim_slider(
                        current_time - const_active_circle_duration,
                        const_active_circle_duration,
                        (float)sf::Mouse::getPosition(window).x,
                        (float)sf::Mouse::getPosition(window).y,
                        const_circle_beat_radius, const_active_circle_radius,
                        (float)sf::Mouse::getPosition(window).x,
                        (float)sf::Mouse::getPosition(window).y,
                        sf::seconds(1000))));
                start_draw_iterator--;
                dragged_pos_ptr = &(*start_draw_iterator)->get_end_pos();
                slider_choosing_end = true;
                break;
            case OBJECT_TO_CREATE::SPINNER:
                break;
        }
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
        if (time_delta > sf::seconds(0) &&
            music.getStatus() == sf::Music::Playing) {
            remembered_time += time_delta;
            music.setPlayingOffset(remembered_time + clock.getElapsedTime());
        }
        if (editing_map_objects.empty()) {
            return;
        }

        while (start_draw_iterator != editing_map_objects.end() &&
               (*start_draw_iterator)->get_start_time() +
                       (*start_draw_iterator)->get_duration_time() +
                       (*start_draw_iterator)->get_move_time() <
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
        if (music.getStatus() == sf::Music::Playing) {
            remembered_time =
                remembered_time + clock.getElapsedTime() - time_delta;
            clock.restart();
            remembered_time = remembered_time < sf::seconds(0)
                                  ? sf::seconds(0)
                                  : remembered_time;
            music.setPlayingOffset(remembered_time + clock.getElapsedTime());
        }

        if (editing_map_objects.empty()) {
            return;
        }

        if (start_draw_iterator == editing_map_objects.end()) {
            start_draw_iterator--;
        }
        while (start_draw_iterator != editing_map_objects.begin() &&
               (*start_draw_iterator)->get_start_time() +
                       (*start_draw_iterator)->get_duration_time() +
                       (*start_draw_iterator)->get_move_time() >
                   current_time) {
            (*start_draw_iterator)->reset();
            start_draw_iterator--;
        }
        if ((*start_draw_iterator)->get_start_time() +
                (*start_draw_iterator)->get_duration_time() +
                (*start_draw_iterator)->get_move_time() <
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
        }
        if (start_draw_iterator == end_draw_iterator) {
            (*start_draw_iterator)->reset();
        }
        end_draw_iterator++;
    };

    auto draw_yes_and_no_decision = [&](const std::string& decision) {
        sf::Text text_buf;
        text_buf.setCharacterSize(window.getSize().y / 10);

        sf::CircleShape yes_button((float)window.getSize().x / 6);
        yes_button.setPosition((float)yes_button.getRadius() * 2,
                               (float)window.getSize().y / 2);
        yes_button.setFillColor(sf::Color::Green);

        sf::CircleShape no_button(yes_button.getRadius());
        no_button.setPosition((float)no_button.getRadius() * 4,
                              (float)window.getSize().y / 2);
        no_button.setFillColor(sf::Color::Red);

        sf::RectangleShape rect(sf::Vector2f(window.getSize()));
        rect.setFillColor(sf::Color(0, 0, 0, 70));
        while (true) {
            window.draw(rect);
            text_buf.setString(decision);
            text_buf.setPosition(sf::Vector2f((float)window.getSize().x * 2 / 5, (float)window.getSize().y / 10));
            window.draw(text_buf);

            window.draw(yes_button);
            window.draw(no_button);

            text_buf.setString("yes");
            text_buf.setPosition(
                yes_button.getPosition() +
                sf::Vector2f(yes_button.getRadius(), yes_button.getRadius()));
            window.draw(text_buf);

            text_buf.setString("no");
            text_buf.setPosition(
                no_button.getPosition() +
                sf::Vector2f(yes_button.getRadius(), yes_button.getRadius()));
            window.draw(text_buf);
            if (!window.pollEvent(event)) {
                continue;
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (is_circle_correct_click(
                    sf::Vector2f(sf::Mouse::getPosition()),
                    yes_button.getPosition() +
                    sf::Vector2f(yes_button.getRadius(),
                                 yes_button.getRadius()),
                    yes_button.getRadius())) {
                    return true;
                } else if (is_circle_correct_click(
                    sf::Vector2f(sf::Mouse::getPosition()),
                    no_button.getPosition() +
                    sf::Vector2f(no_button.getRadius(),
                                 no_button.getRadius()),
                    no_button.getRadius())) {
                    return false;
                }
            }
            window.display();
        }
    };
    auto draw_input_text_box = [&](sf::Time &time,
                             std::string time_name) {
        sf::Event event{};
        time_name += "=";
        sf::RectangleShape input_field(
        sf::Vector2f((float)window.getSize().x / 4, 90));
        input_field.setFillColor(sf::Color::White);
        input_field.setPosition(sf::Vector2f(window.getSize()) / 2.0f -
                                sf::Vector2f(input_field.getSize()) / 2.0f);
        sf::Int32 input_in_int = 0;
        std::string input;
        sf::Text genius_text;
        genius_text.setPosition(input_field.getPosition());
        genius_text.setFont(font);
        genius_text.setCharacterSize(70);
        genius_text.setFillColor(sf::Color::Red);
        while (true) {
            if (!window.pollEvent(event)) {
                continue;
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    return;
                } else if (event.key.code == sf::Keyboard::Enter) {
                    time = sf::milliseconds(input_in_int);
                    break;
                } else if (event.key.code == sf::Keyboard::BackSpace) {
                    input_in_int /= 10;
                    if (!input.empty()) {
                        input.pop_back();
                    }
                }
            } else if (event.type == sf::Event::TextEntered) {
                if (input.size() < 5 &&
                    std::isdigit(static_cast<char>(event.text.unicode))) {
                    input_in_int *= 10;
                    input_in_int += static_cast<char>(event.text.unicode) - '0';
                    input += static_cast<char>(event.text.unicode);
                }
            }
            genius_text.setString(time_name + input);
            window.draw(input_field);
            window.draw(genius_text);
            window.display();
        }
    };
    // lambda zone ends

    while (true) {
        if (music.getStatus() == sf::Music::Playing) {
            current_time = clock.getElapsedTime() + remembered_time;
            time_delta = sf::seconds(0);
            inc_time();
            time_delta = saved_delta;
        }
        window.draw(rect);

        for (auto i = start_draw_iterator; i != end_draw_iterator; i++) {
            if ((*i)->get_start_time() > current_time) {
                continue;
            }
            (*i)->change_state(current_time);
            (*i)->draw(window, font);
        }

        {
            sf::RectangleShape object_rect;
            object_rect.setFillColor(sf::Color::Yellow);
            for (auto i = start_draw_iterator;
                 i != editing_map_objects.end() &&
                 (*i)->get_start_time() + (*i)->get_duration_time() <
                     current_time + time_delta * (float)number_of_delta;
                 i++) {
                float pos_coef = ((*i)->get_start_time() +
                                  (*i)->get_duration_time() - current_time) /
                                 ((float)number_of_delta * time_delta);
                float width_coef = ((*i)->get_move_time()) /
                                   ((float)number_of_delta * time_delta);
                object_rect.setPosition({road_rect.getPosition().x +
                                             pos_coef * road_rect.getSize().x,
                                         road_rect.getPosition().y});
                object_rect.setSize(
                    {std::max(10.f, width_coef * road_rect.getSize().x) +
                         std::min(0.f, object_rect.getPosition().x -
                                           road_rect.getPosition().x),
                     road_rect.getSize().y});
                object_rect.setPosition({std::max(road_rect.getPosition().x,
                                                  object_rect.getPosition().x),
                                         object_rect.getPosition().y});
                window.draw(object_rect);
            }
            if (break_point_time >= current_time &&
                break_point_time - current_time <
                    (float)number_of_delta * time_delta) {
                object_rect.setFillColor(sf::Color::Red);
                object_rect.setSize({20.f, road_rect.getSize().y});
                object_rect.setPosition(
                    road_rect.getPosition().x +
                        (break_point_time - current_time) /
                            (time_delta * (float)number_of_delta) *
                            road_rect.getSize().x,
                    road_rect.getPosition().y);
                window.draw(object_rect);
            }
            window.draw(road_rect);
        }

        {
            sf::Int64 min = current_time.asMilliseconds() /
                            sf::seconds(60).asMilliseconds();
            sf::Int64 sec = current_time.asMilliseconds() /
                            sf::seconds(1).asMilliseconds() % 60;
            sf::Int64 millisec =
                current_time.asMilliseconds() % sf::seconds(1).asMilliseconds();

            current_time_text.setString(std::to_string(min) + ":" +
                                        std::to_string(sec) + ":" +
                                        std::to_string(millisec));
            window.draw(current_time_text);
        }

        delta_text.setString("Delta = " +
                             std::to_string(time_delta.asMilliseconds()));
        window.draw(delta_text);

        window.display();

        if (!window.pollEvent(event) && !drag) {
            continue;
        }

        switch (event.type) {
            case sf::Event::KeyPressed:
                if (dragged_pos_ptr && drag) {
                    *dragged_pos_ptr =
                        sf::Vector2f(sf::Mouse::getPosition(window));
                    break;
                }
                if (event.key.code == sf::Keyboard::Escape) {
                    music.pause();
                    remembered_time = current_time;
                    if (draw_yes_and_no_decision("Do you want to save?")) {
                        std::fstream fout;
                        fout.open(R"(data/maps/editing_map.txt)",
                                  std::ios::out | std::ios::trunc);
                        if (!fout) {
                            std::cerr << "File not found" << std::endl;
                        }
                        fout << map_name << std::endl;
                        fout << author_name << std::endl;
                        fout << music_address << std::endl;
                        fout << music_name << std::endl;
                        fout << image_address << std::endl;
                        fout << font_address << std::endl;
                        fout << sound_address << std::endl;
                        for (auto &i : editing_map_objects) {
                            auto &object = *i;
                            if (typeid(object) == typeid(USO::Aim_circle)) {
                                fout << "Aim_circle" << std::endl;
                                fout << i->get_start_time().asMicroseconds()
                                     << std::endl;
                                fout << i->get_duration_time().asMicroseconds()
                                     << std::endl;
                                fout << i->get_pos().x << std::endl;
                                fout << i->get_pos().y << std::endl;
                                fout << const_circle_beat_radius << " "
                                     << const_active_circle_radius << std::endl;
                            } else if (typeid(object) ==
                                       typeid(USO::Aim_slider)) {
                                fout << "Aim_slider" << std::endl;
                                fout << i->get_start_time().asMicroseconds()
                                     << std::endl;
                                fout << i->get_duration_time().asMicroseconds()
                                     << std::endl;
                                fout << i->get_pos().x << std::endl;
                                fout << i->get_pos().y << std::endl;
                                fout << const_circle_beat_radius << " "
                                     << const_active_circle_radius << std::endl;
                                fout << i->get_end_pos().x << " "
                                     << i->get_end_pos().y << std::endl;
                                fout << i->get_move_time().asMicroseconds()
                                     << std::endl;
                            }
                        }
                        fout.close();
                    }
                    if (draw_yes_and_no_decision("Do you want to leave")) {
                        return;
                    }
                }
                if (event.key.code == sf::Keyboard::Space) {
                    if (music.getStatus() == sf::Music::Paused ||
                        music.getStatus() == sf::Music::Stopped) {
                        remembered_time = current_time;
                        saved_delta = time_delta;
                        clock.restart();
                        music.setPlayingOffset(current_time);
                        music.play();
                    } else {
                        time_delta = saved_delta;
                        music.pause();
                    }
                    break;
                }
                if (event.key.code == sf::Keyboard::Right) {
                    inc_time();
                } else if (event.key.code == sf::Keyboard::Left) {
                    dec_time();
                } else if (event.key.code == sf::Keyboard::Z) {
                    dragged_key[event.key.code] = 1;
                    handle_left_click();
                } else if (event.key.code == sf::Keyboard::X) {
                    handle_right_click();
                } else if (event.key.code == sf::Keyboard::D) {
                    music.pause();
                    draw_input_text_box(time_delta, "delta");
                } else if (event.key.code == sf::Keyboard::B) {
                    break_point_time = current_time;
                } else if (event.key.code == sf::Keyboard::R) {
                    remembered_time = break_point_time;
                    clock.restart();
                    current_time = remembered_time;
                    saved_delta = time_delta;
                    time_delta = sf::seconds(0);
                    dec_time();
                    inc_time();
                    time_delta = saved_delta;
                } else if (event.key.code == sf::Keyboard::Num1) {
                    object_to_create = OBJECT_TO_CREATE::CIRCLE;
                } else if (event.key.code == sf::Keyboard::Num2) {
                    object_to_create = OBJECT_TO_CREATE::SLIDER;
                } else if (event.key.code == sf::Keyboard::Num3) {
                    object_to_create = OBJECT_TO_CREATE::SPINNER;
                }
                break;
            case sf::Event::MouseMoved:
                if (dragged_pos_ptr && drag) {
                    *dragged_pos_ptr =
                        sf::Vector2f(sf::Mouse::getPosition(window));
                    break;
                }

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
                dragged_pos_ptr = nullptr;
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
                if (object_to_create == OBJECT_TO_CREATE::SLIDER) {
                    if (!dragged_pos_ptr) {
                        break;
                    }
                    *dragged_pos_ptr = sf::Vector2f(sf::Mouse::getPosition());
                    (*start_draw_iterator)->get_move_time() =
                        sf::seconds(static_cast<float>(
                            get_dist(dynamic_cast<Aim_slider &>(
                                         **start_draw_iterator)
                                         .get_start_pos(),
                                     (*start_draw_iterator)->get_end_pos()) *
                            time_per_pixels / 1000));
                    dragged_pos_ptr = nullptr;
                }
                slider_choosing_end = false;
                break;
            case sf::Event::MouseWheelMoved:
                if (event.mouseWheel.delta > 0) {
                    inc_time();
                } else {
                    dec_time();
                }
                break;
            default:
                if (drag && slider_choosing_end) {
                    (**start_draw_iterator).get_end_pos() =
                        sf::Vector2f(sf::Mouse::getPosition(window));
                }
                break;
        }
    }
}
}  // namespace USO