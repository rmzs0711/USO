#include <cassert>
#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include "SFML/Graphics.hpp"
#include "base_logic.h"
#include "map_objects.h"
#include "maps.h"
#include <menu.h>

namespace {
enum class OBJECT_TO_CREATE { CIRCLE, SLIDER, SPINNER };

struct Editing_box {
    static const int number_of_delta = 10;

    std::list<std::shared_ptr<USO::Map_object>> editing_map_objects;
    sf::RenderWindow &window;

    std::list<std::shared_ptr<USO::Map_object>>::iterator end_draw_iterator;
    std::list<std::shared_ptr<USO::Map_object>>::iterator start_draw_iterator;

    sf::RectangleShape background;
    sf::RectangleShape road_rect;
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
    sf::Text delta_text;

    sf::Sound sound;

    bool drag = true;

    sf::Vector2f *dragged_pos_ptr = nullptr;
    OBJECT_TO_CREATE object_to_create = OBJECT_TO_CREATE::CIRCLE;

    Editing_box(std::list<std::shared_ptr<USO::Map_object>> &map_objects,
                const USO::Map &map,
                sf::RenderWindow &window_)
        : window(window_),
          background(sf::Vector2f(window.getSize())),
          road_rect(sf::Vector2f((float)window.getSize().x / 2, 80)) {
        for (std::shared_ptr<USO::Map_object> &ptr : map_objects) {
            editing_map_objects.emplace_back(ptr->clone());
        }
        start_draw_iterator = editing_map_objects.begin();
        end_draw_iterator = editing_map_objects.begin();
        map.prelude(music, sound_buf, image, font);
        background.setPosition(0, 0);
        background.setTexture(&image);

        road_rect.setPosition(
            (float)window.getSize().x / 2,
            (float)window.getSize().y - road_rect.getSize().y);
        road_rect.setFillColor(sf::Color(10, 145, 9, 50));
        road_rect.setOutlineThickness(2);
        road_rect.setOutlineColor(USO::purple);

        current_time_text.setCharacterSize(80);
        current_time_text.setPosition(0, 0);
        current_time_text.setFont(font);
        current_time_text.setOutlineThickness(5);
        current_time_text.setOutlineColor(USO::purple);
        current_time_text.setFillColor(sf::Color::White);

        delta_text.setCharacterSize(80);
        delta_text.setPosition(
            0, sf::Vector2f(window.getSize()).y -
                   static_cast<float>(delta_text.getCharacterSize()));
        delta_text.setFont(font);
        delta_text.setFillColor(sf::Color::White);
        delta_text.setOutlineThickness(5);
        delta_text.setLetterSpacing(3);
        delta_text.setOutlineColor(USO::purple);

        sound.setBuffer(sound_buf);
    }

    void inc_time() {
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
    void dec_time() {
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
    bool draw_yes_and_no_decision(const std::string &decision) {
        sf::Text text_buf;
        text_buf.setCharacterSize(80);
        text_buf.setFont(font);

        sf::CircleShape yes_button((float)window.getSize().x / 10);
        yes_button.setPosition((float)yes_button.getRadius() * 6,
                               (float)window.getSize().y / 2);
        yes_button.setFillColor(sf::Color::Green);

        sf::CircleShape no_button(yes_button.getRadius());
        no_button.setPosition((float)no_button.getRadius() * 2,
                              (float)window.getSize().y / 2);
        no_button.setFillColor(sf::Color::Red);

        background.setFillColor(sf::Color(100, 100, 100, 10));
        while (true) {
            window.draw(background);
            text_buf.setString(decision);
            text_buf.setFillColor(sf::Color::White);
            text_buf.setPosition(sf::Vector2f(
                (float)window.getSize().x / 2 -
                    (float)(decision.size() / 4 * text_buf.getCharacterSize()),
                (float)window.getSize().y / 10));
            window.draw(text_buf);

            window.draw(yes_button);
            window.draw(no_button);

            text_buf.setString("yes");
            text_buf.setPosition(
                yes_button.getPosition() +
                sf::Vector2f(
                    yes_button.getRadius() - text_buf.getCharacterSize() / 2,
                    yes_button.getRadius() - text_buf.getCharacterSize() / 2));
            window.draw(text_buf);

            text_buf.setString("no");
            text_buf.setPosition(
                no_button.getPosition() +
                sf::Vector2f(
                    yes_button.getRadius() - text_buf.getCharacterSize() / 2,
                    yes_button.getRadius() - text_buf.getCharacterSize() / 2));
            window.draw(text_buf);
            window.display();
            sf::Event event{};
            if (!window.pollEvent(event)) {
                continue;
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                background.setFillColor(sf::Color::White);
                if (USO::Aim_circle::is_circle_correct_click(
                        sf::Vector2f(sf::Mouse::getPosition(window)),
                        no_button.getPosition() +
                            sf::Vector2f(no_button.getRadius(),
                                         no_button.getRadius()),
                        no_button.getRadius())) {
                    return false;
                } else if (USO::Aim_circle::is_circle_correct_click(
                               sf::Vector2f(sf::Mouse::getPosition(window)),
                               yes_button.getPosition() +
                                   sf::Vector2f(yes_button.getRadius(),
                                                yes_button.getRadius()),
                               yes_button.getRadius())) {
                    return true;
                }
            }
        }
    }

    void draw_input_text_box(sf::Time &time, std::string time_name) {
        sf::Event event{};
        music.pause();
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
    }

    void change_state() {
        if (music.getStatus() == sf::Music::Playing) {
            current_time = clock.getElapsedTime() + remembered_time;
            time_delta = sf::seconds(0);
            inc_time();
            time_delta = saved_delta;
        }
    }
    void draw_background() {
        window.draw(background);
    }
    void draw_objects() {
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
    }
    void play_or_pause() {
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
    }
    void return_to_breakpoint() {
        remembered_time = break_point_time;
        clock.restart();
        current_time = remembered_time;
        saved_delta = time_delta;
        time_delta = sf::seconds(0);
        dec_time();
        inc_time();
        time_delta = saved_delta;
    }
};

}  // namespace

namespace USO {
void Aim_map::constructor_run(sf::RenderWindow &window) {
    Editing_box editing_box(map_objects, *this, window);

    bool slider_choosing_end = false;

    sf::Event event{};
    bool is_saved;

    // WARNING: lambda zone
    auto handle_left_click = [&]() {
        if (editing_box.drag) {
            return;
        }
        is_saved = false;
        editing_box.drag = true;

        editing_box.music.pause();
        for (auto i = editing_box.start_draw_iterator;
             i != editing_box.end_draw_iterator; i++) {
            auto &object = **i;
            if (USO::Aim_circle::is_circle_correct_click(
                    sf::Vector2f(sf::Mouse::getPosition(window)),
                    (*i)->get_end_pos(), USO::const_circle_beat_radius)) {
                editing_box.dragged_pos_ptr = (*i)->get_end_pos_ptr();
                return;
            }
        }
        switch (editing_box.object_to_create) {
            case OBJECT_TO_CREATE::CIRCLE:
                editing_box.editing_map_objects.insert(
                    editing_box.start_draw_iterator,
                    std::make_shared<USO::Aim_circle>(
                        USO::Aim_circle(editing_box.current_time -
                                            USO::const_active_circle_duration,
                                        USO::const_active_circle_duration,
                                        (float)sf::Mouse::getPosition(window).x,
                                        (float)sf::Mouse::getPosition(window).y,
                                        USO::const_circle_beat_radius,
                                        USO::const_active_circle_radius)));
                editing_box.start_draw_iterator--;
                editing_box.dragged_pos_ptr =
                    (*editing_box.start_draw_iterator)->get_end_pos_ptr();
                break;
            case OBJECT_TO_CREATE::SLIDER:

                editing_box.editing_map_objects.insert(
                    editing_box.start_draw_iterator,
                    std::make_shared<USO::Aim_slider>(
                        USO::Aim_slider(editing_box.current_time -
                                            USO::const_active_circle_duration,
                                        USO::const_active_circle_duration,
                                        (float)sf::Mouse::getPosition(window).x,
                                        (float)sf::Mouse::getPosition(window).y,
                                        USO::const_circle_beat_radius,
                                        USO::const_active_circle_radius,
                                        (float)sf::Mouse::getPosition(window).x,
                                        (float)sf::Mouse::getPosition(window).y,
                                        sf::seconds(1000))));
                editing_box.start_draw_iterator--;
                editing_box.dragged_pos_ptr =
                    (*editing_box.start_draw_iterator)->get_end_pos_ptr();
                slider_choosing_end = true;
                break;
            case OBJECT_TO_CREATE::SPINNER:
                editing_box.editing_map_objects.insert(
                    editing_box.start_draw_iterator,
                    std::make_shared<USO::Aim_spinner>(USO::Aim_spinner(
                        editing_box.current_time -
                            USO::const_active_circle_duration * 2.f,
                        USO::const_active_circle_duration * 2.f,
                        (float)window.getSize().x / 2,
                        (float)window.getSize().y / 2,
                        USO::const_active_circle_radius * 2)));
                editing_box.start_draw_iterator--;
                editing_box.dragged_pos_ptr =
                    (*editing_box.start_draw_iterator)->get_end_pos_ptr();
                break;
        }
    };

    auto handle_right_click = [&]() {
        is_saved = false;
        for (auto i = editing_box.start_draw_iterator;
             i != editing_box.end_draw_iterator; i++) {
            if (USO::Aim_circle::is_circle_correct_click(
                    sf::Vector2f(sf::Mouse::getPosition(window)),
                    (*i)->get_pos(), USO::const_circle_beat_radius)) {
                if (i == editing_box.start_draw_iterator) {
                    editing_box.start_draw_iterator++;
                }
                editing_box.editing_map_objects.erase(i);
                return;
            }
        }
    };

    auto save_map = [&]() {
        std::fstream fout;
        fout.open(map_address, std::ios::out | std::ios::trunc);
        if (!fout) {
            std::cerr << "File not found" << std::endl;
            return;
        }
        is_saved = true;
        fout << "Aim" << std::endl;
        fout << map_name << std::endl;
        fout << map_address << std::endl;
        fout << music_address << std::endl;
        fout << music_name << std::endl;
        fout << image_address << std::endl;
        fout << font_address << std::endl;
        fout << sound_address << std::endl;
        for (auto &i : editing_box.editing_map_objects) {
            auto &object = *i;
            if (typeid(object) == typeid(USO::Aim_circle)) {
                fout << "Aim_circle" << std::endl;
                fout << i->get_start_time().asMicroseconds() << std::endl;
                fout << i->get_duration_time().asMicroseconds() << std::endl;
                fout << i->get_pos().x << std::endl;
                fout << i->get_pos().y << std::endl;
                fout << const_circle_beat_radius << " "
                     << const_active_circle_radius << std::endl;
            } else if (typeid(object) == typeid(USO::Aim_slider)) {
                fout << "Aim_slider" << std::endl;
                fout << i->get_start_time().asMicroseconds() << std::endl;
                fout << i->get_duration_time().asMicroseconds() << std::endl;
                fout << i->get_pos().x << std::endl;
                fout << i->get_pos().y << std::endl;
                fout << const_circle_beat_radius << " "
                     << const_active_circle_radius << std::endl;
                fout << i->get_end_pos().x << " " << i->get_end_pos().y
                     << std::endl;
                fout << i->get_move_time().asMicroseconds() << std::endl;
            } else if (typeid(object) == typeid(USO::Aim_spinner)) {
                fout << "Aim_spinner" << std::endl;
                fout << i->get_start_time().asMicroseconds() << std::endl;
                fout << i->get_duration_time().asMicroseconds() << std::endl;
                fout << i->get_pos().x << std::endl;
                fout << i->get_pos().y << std::endl;
                fout << const_active_circle_radius * 2 << std::endl;
            }
        }
        map_objects = editing_box.editing_map_objects;
        fout.close();
    };
    // lambda zone ends

    while (true) {
        editing_box.change_state();
        editing_box.draw_background();
        editing_box.draw_objects();
        window.display();

        if (!window.pollEvent(event) && !editing_box.drag) {
            continue;
        }

        switch (event.type) {
            case sf::Event::KeyPressed:
                if (editing_box.dragged_pos_ptr && editing_box.drag) {
                    *editing_box.dragged_pos_ptr =
                        sf::Vector2f(sf::Mouse::getPosition(window));
                    break;
                }
                if (event.key.code == sf::Keyboard::Escape) {
                    editing_box.music.pause();
                    editing_box.remembered_time = editing_box.current_time;
                    if (!is_saved &&
                        editing_box.draw_yes_and_no_decision("Save?")) {
                        save_map();
                    }
                    window.display();
                    if (editing_box.draw_yes_and_no_decision(
                            "Do you want to leave?")) {
                        return;
                    }
                    event = sf::Event();
                    break;
                }
                if (event.key.code == sf::Keyboard::Space) {
                    editing_box.play_or_pause();
                    break;
                }
                if (event.key.code == sf::Keyboard::Right) {
                    editing_box.inc_time();
                } else if (event.key.code == sf::Keyboard::Left) {
                    editing_box.dec_time();
                } else if (event.key.code == sf::Keyboard::Z) {
                    handle_left_click();
                } else if (event.key.code == sf::Keyboard::X) {
                    handle_right_click();
                } else if (event.key.code == sf::Keyboard::D) {
                    editing_box.draw_input_text_box(editing_box.time_delta,
                                                    "delta");
                } else if (event.key.code == sf::Keyboard::B) {
                    editing_box.break_point_time = editing_box.current_time;
                } else if (event.key.code == sf::Keyboard::R) {
                    editing_box.return_to_breakpoint();
                } else if (event.key.code == sf::Keyboard::S) {
                    if (!is_saved) {
                        save_map();
                    }
                } else if (event.key.code == sf::Keyboard::Num1) {
                    editing_box.object_to_create = OBJECT_TO_CREATE::CIRCLE;
                } else if (event.key.code == sf::Keyboard::Num2) {
                    editing_box.object_to_create = OBJECT_TO_CREATE::SLIDER;
                } else if (event.key.code == sf::Keyboard::Num3) {
                    editing_box.object_to_create = OBJECT_TO_CREATE::SPINNER;
                }
                break;
            case sf::Event::MouseMoved:
                if (editing_box.dragged_pos_ptr && editing_box.drag) {
                    *editing_box.dragged_pos_ptr =
                        sf::Vector2f(sf::Mouse::getPosition(window));
                    break;
                }

            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button != sf::Mouse::Left &&
                    event.mouseButton.button != sf::Mouse::Right) {
                    break;
                }
                if (event.mouseButton.button == sf::Mouse::Left) {
                    handle_left_click();
                } else {
                    handle_right_click();
                }
                break;
            case sf::Event::KeyReleased:
                if (!editing_box.drag) {
                    break;
                }
                if (event.key.code != sf::Keyboard::Z &&
                    event.key.code != sf::Keyboard::X) {
                    break;
                }
                editing_box.drag = false;
                if (editing_box.object_to_create == OBJECT_TO_CREATE::SLIDER) {
                    if (!editing_box.dragged_pos_ptr) {
                        break;
                    }
                    *editing_box.dragged_pos_ptr =
                        sf::Vector2f(sf::Mouse::getPosition());
                    for (auto i = editing_box.start_draw_iterator;
                         i != editing_box.end_draw_iterator; i++) {
                        (*i)->reset();
                    }
                    editing_box.dragged_pos_ptr = nullptr;
                }
                slider_choosing_end = false;
                break;
                break;
            case sf::Event::MouseButtonReleased:
                if (!editing_box.drag) {
                    break;
                }
                editing_box.drag = false;
                if (editing_box.object_to_create == OBJECT_TO_CREATE::SLIDER) {
                    if (!editing_box.dragged_pos_ptr) {
                        break;
                    }
                    *editing_box.dragged_pos_ptr =
                        sf::Vector2f(sf::Mouse::getPosition());
                    for (auto i = editing_box.start_draw_iterator;
                         i != editing_box.end_draw_iterator; i++) {
                        (*i)->reset();
                    }
                    editing_box.dragged_pos_ptr = nullptr;
                }
                slider_choosing_end = false;
                break;
            case sf::Event::MouseWheelMoved:
                if (event.mouseWheel.delta > 0) {
                    editing_box.inc_time();
                } else {
                    editing_box.dec_time();
                }
                break;
            default:
                break;
        }
    }
}
void Conveyor_map::constructor_run(sf::RenderWindow &window) {
    Editing_box editing_box(map_objects, *this, window);

    sf::Event event{};
    bool is_saved = false;

    // WARNING: lambda zone
    auto handle_click = [&](USO::Conveyor_line &line) {
        if (line.dragged) {
            return;
        }
        line.dragged = true;
        is_saved = false;

        editing_box.music.pause();
        if (editing_box.start_draw_iterator !=
                editing_box.editing_map_objects.end() &&
            USO::Conveyor_note::is_note_correct_click(
                line.beat_pos, (*editing_box.start_draw_iterator)->get_pos(),
                dynamic_cast<USO::Conveyor_note &>(
                    **editing_box.start_draw_iterator)
                    .get_line())) {
            editing_box.editing_map_objects.erase(
                editing_box.start_draw_iterator);
            editing_box.start_draw_iterator++;
            return;
        }

        editing_box.editing_map_objects.insert(
            editing_box.start_draw_iterator,
            std::make_shared<USO::Conveyor_note>(USO::Conveyor_note(
                editing_box.current_time - USO::const_active_note_duration,
                USO::const_active_note_duration, line)));
        editing_box.start_draw_iterator--;
    };

    auto save_map = [&]() {
        std::fstream fout;
        fout.open(map_address, std::ios::out | std::ios::trunc);
        if (!fout) {
            std::cerr << "File not found" << std::endl;
            return;
        }
        is_saved = true;
        fout << "Conveyor" << std::endl;
        fout << map_name << std::endl;
        fout << map_address << std::endl;
        fout << music_address << std::endl;
        fout << music_name << std::endl;
        fout << image_address << std::endl;
        fout << font_address << std::endl;
        fout << sound_address << std::endl;

        for (auto &i : editing_box.editing_map_objects) {
            auto &object = *i;
            fout << i->get_start_time().asMicroseconds() << std::endl;
            fout << i->get_duration_time().asMicroseconds() << std::endl;
            fout << dynamic_cast<USO::Conveyor_note &>(*i).get_line().index
                 << std::endl;
        }
        map_objects = editing_box.editing_map_objects;
        fout.close();
    };
    // lambda zone ends

    while (true) {
        editing_box.change_state();

        editing_box.draw_background();
        for (auto &line : lines) {
            line->draw(window);
        }
        editing_box.draw_objects();
        window.display();

        if (!window.pollEvent(event)) {
            continue;
        }

        switch (event.type) {
            case sf::Event::KeyPressed:
                if (editing_box.dragged_pos_ptr && editing_box.drag) {
                    *editing_box.dragged_pos_ptr =
                        sf::Vector2f(sf::Mouse::getPosition(window));
                    break;
                }
                if (event.key.code == sf::Keyboard::Escape) {
                    editing_box.music.pause();
                    editing_box.remembered_time = editing_box.current_time;
                    if (!is_saved &&
                        editing_box.draw_yes_and_no_decision("Save?")) {
                        save_map();
                    }
                    window.display();
                    if (editing_box.draw_yes_and_no_decision(
                            "Do you want to leave")) {
                        return;
                    }
                }
                if (event.key.code == sf::Keyboard::Space) {
                    editing_box.play_or_pause();
                    break;
                }
                if (event.key.code == sf::Keyboard::Right) {
                    editing_box.inc_time();
                } else if (event.key.code == sf::Keyboard::Left) {
                    editing_box.dec_time();
                } else if (event.key.code == sf::Keyboard::D) {
                    editing_box.draw_input_text_box(editing_box.time_delta,
                                                    "delta");
                } else if (event.key.code == sf::Keyboard::B) {
                    editing_box.break_point_time = editing_box.current_time;
                } else if (event.key.code == sf::Keyboard::R) {
                    editing_box.return_to_breakpoint();
                } else if (event.key.code == sf::Keyboard::S) {
                    if (!is_saved) {
                        save_map();
                    }
                } else if (event.key.code >= sf::Keyboard::Num1 &&
                           event.key.code <= sf::Keyboard::Num4) {
                    handle_click(*lines[event.key.code - sf::Keyboard::Num1]);
                }
                break;
            case sf::Event::KeyReleased:
                if (event.key.code >= sf::Keyboard::Num1 &&
                    event.key.code <= sf::Keyboard::Num4) {
                    lines[event.key.code - sf::Keyboard::Num1]->dragged = false;
                }
                break;
            case sf::Event::MouseWheelMoved:
                if (event.mouseWheel.delta > 0) {
                    editing_box.inc_time();
                } else {
                    editing_box.dec_time();
                }
                break;
            default:
                break;
        }
    }
}
}  // namespace USO
