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
#include "menu.h"

namespace {}  // namespace

void table_of_scores(sf::RenderWindow &window,
                     sf::Font &font,
                     BL::Game_session &game_session) {
    /*sf::RectangleShape table;
    table.setSize(sf::Vector2f(250.f, 120.f));
    table.setFillColor(sf::Color::Blue);
    table.setOutlineThickness(5.f);
    table.setOutlineColor(sf::Color);
    window.draw(table);*/
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(30);
    //    text.setFillColor(sf::Color::White);
    text.setStyle(sf::Text::Bold);
    text.setString("SCORE: " + std::to_string(game_session.get_score()));
    window.draw(text);
    text.setPosition(sf::Vector2f(0.f, 35.f));
    text.setString("COMBO: " + std::to_string(game_session.get_combo()));
    window.draw(text);
    text.setPosition(sf::Vector2f(0.f, 70.f));
    text.setString("HEALTH: " + std::to_string(game_session.get_health()));
    window.draw(text);
}

void USO::Aim_map::run(sf::RenderWindow & window) {
    BL::Game_session game_session;
    USO::Field field(window, {});
    sf::Clock clock;
    sf::SoundBuffer press_sound;
    sf::Time past_time;  // костыль для паузы, так как sfml не умеет
                         // останавливать часы
    window.setMouseCursorVisible(false);
    auto current_object_it =
        map_objects.begin();  // итератор на следующий по времени объект
    sf::Music music;

    check_file_load(press_sound.loadFromFile(R"(data\music\click_sound.ogg)"),
                    R"(data\music\click_sound.ogg)");
    music.openFromFile(music_address);
    music.play();

    sf::Sound sound;
    sound.setBuffer(press_sound);

    sf::Texture img;
    check_file_load(
        img.loadFromFile(R"(data\img\stronger.png)"),
        R"(data\img\stronger.png)");  //Тут нужно сделать загрузку названия из
                                      //карты, если карта содержит в себе фон

    sf::RectangleShape rect(static_cast<sf::Vector2f>(window.getSize()));
    rect.setPosition(0, 0);
    rect.setTexture(&img);

    sf::Font font;
    check_file_load(font.loadFromFile(R"(data\fonts\GistLight.otf)"),
                    R"(data\fonts\GistLight.otf)");

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(42);
    text.setStyle(sf::Text::Bold);

    assert(game_session.get_game_status() == BL::Game_status::ACTION);
    bool drag = false;
    std::vector<int> dragged_key(
        sf::Keyboard::KeyCount);  // чтобы различать какая кнопка зажата
    std::vector<int> dragged_mouse_button(
        sf::Mouse::ButtonCount);  // то же самое только про мышку

    clock.restart();
    sf::CircleShape mouse(5.f);
    mouse.setFillColor(sf::Color(241, 200, 14));

    while (true) {
        window.draw(rect);
        mouse.setPosition((sf::Vector2f)sf::Mouse::getPosition());
        window.draw(mouse);
        table_of_scores(window, font, game_session);

        if (game_session.get_health() == 0) game_session.set_game_status(BL::Game_status::DEFEAT);
        if (map_objects.back()->get_start_time() +
                map_objects.back()->get_duration_time() < past_time + clock.getElapsedTime()) {
            game_session.set_game_status(BL::Game_status::VICTORY);
        }

        sf::Event event{};
        switch (game_session.get_game_status()) {
            case BL::Game_status::ACTION: {
                if (current_object_it != map_objects.end()) {
                    if (*current_object_it) {
                        field.push_front(current_object_it,
                                   past_time + clock.getElapsedTime());
                    } else {
                        std::cerr << "invalid object iterator" << std::endl;
                        current_object_it++;
                    }
                }

                field.change_state(past_time + clock.getElapsedTime());

                if (!field.get_field_objects().empty() &&
                    !field.get_field_objects().back()->change_state(
                        past_time + clock.getElapsedTime())) {
                    field.get_field_objects().pop_back();
                    game_session.decrease_health(game_session.damage());
                }



                field.draw(font);
                mouse.setPosition((sf::Vector2f)sf::Mouse::getPosition());
                window.draw(mouse);
                window.display();

                if (!window.pollEvent(event) && !drag) {
                    continue;
                }

                // WARNING: lambda zone
                auto handle_click = [&]() -> void {
                    drag = true;  //Зажимаю мышку
                    if (field.get_field_objects().empty()) {
                        return;
                    }
                    USO::Map_object &front_object =
                        *(field.get_field_objects().front().get());
                    if (!(*(field.get_field_objects().back()))
                             .check_event(static_cast<sf::Vector2f>(
                                              sf::Mouse::getPosition()),
                                          game_session,
                                          past_time + clock.getElapsedTime())) {
                        return;
                    }
                    if (typeid(front_object) != typeid(USO::Aim_slider) &&
                        typeid(front_object) != typeid(USO::Aim_spinner)) {
                        field.get_field_objects().pop_back();
                    }
                    sound.play();
                };
                // lambda zone ends

                switch (event.type) {
                    case sf::Event::KeyPressed:
                        if (event.key.code == sf::Keyboard::Escape) {
                            game_session.set_game_status(BL::Game_status::PAUSE);
                            continue;
                        }
                        if (event.key.code != sf::Keyboard::Z &&
                            event.key.code != sf::Keyboard::X) {
                            break;
                        }
                        dragged_key[event.key.code] = 1;
                        handle_click();
                        break;
                    case sf::Event::MouseButtonPressed:
                        if (event.mouseButton.button != sf::Mouse::Left &&
                            event.mouseButton.button != sf::Mouse::Right) {
                            break;
                        }
                        dragged_mouse_button[event.mouseButton.button] = 1;
                        handle_click();
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
                        if (dragged_mouse_button[event.mouseButton.button] ==
                            0) {
                            break;
                        }
                        dragged_mouse_button[event.mouseButton.button] = 0;
                        drag = false;
                        break;
                    default:
                        if (drag && !field.get_field_objects().empty()) {
                            USO::Map_object &front_object =
                                *(field.get_field_objects().front().get());
                            if (typeid(front_object) !=
                                typeid(USO::Aim_slider) &&
                                typeid(front_object) != typeid(USO::Aim_spinner)) {
                                break;
                            }
                            front_object.check_event(
                                sf::Vector2f(sf::Mouse::getPosition()),
                                game_session,
                                past_time + clock.getElapsedTime());
                        }
                        break;
                }
                break;
            }
            case BL::Game_status::PAUSE: {
                past_time += clock.getElapsedTime();
                mouse.setPosition((sf::Vector2f)sf::Mouse::getPosition());
                window.draw(mouse);
                Menu::stop_menu(window, game_session);
                clock.restart();
                if (game_session.get_game_status() != BL::Game_status::ACTION) {
                    return;
                }
            } break;
            case BL::Game_status::VICTORY: {
                while (true) {
                    if (window.pollEvent(event)) {
                        if (event.key.code == sf::Keyboard::Space) {
                            field.get_field_objects().clear();
                            map_objects.clear();
                            mouse.setPosition((sf::Vector2f)sf::Mouse::getPosition());
                            window.draw(mouse);
                            Menu::stop_menu(window, game_session);
                            return;
                        }
                    }
                    window.draw(rect);
                    table_of_scores(window, font, game_session);
                    mouse.setPosition((sf::Vector2f)sf::Mouse::getPosition());
                    window.draw(mouse);
                    window.display();
                }
            } break;
            case BL::Game_status::DEFEAT: {
                field.get_field_objects().clear();
                map_objects.clear();
                mouse.setPosition((sf::Vector2f)sf::Mouse::getPosition());
                window.draw(mouse);
                Menu::stop_menu(window, game_session);
                return;
            }
            default: {
                continue;
            }
        }
    }
}
