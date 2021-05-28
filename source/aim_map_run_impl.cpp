#include <cassert>
#include <functional>
#include <iostream>
#include "SFML/Graphics.hpp"
#include "base_logic.h"
#include "map_management.h"
#include "maps.h"
#include "menu.h"

namespace {}  // namespace

void USO::Aim_map::run(sf::RenderWindow &window) {
    BL::Game_session game_session;
    USO::Field field(window, {});
    sf::Clock clock;
    sf::Time past_time;  // костыль для паузы, так как sfml не умеет
                         // останавливать часы
    window.setMouseCursorVisible(false);
    auto current_object_it =
        map_objects.begin();  // итератор на следующий по времени объект

    sf::Music music;
    sf::SoundBuffer sound_buf;
    sf::Texture image;
    sf::Font font;

    prelude(music, sound_buf, image, font);  // Загружает и проверяет

    sf::Sound sound;
    sound.setBuffer(sound_buf);

    sf::RectangleShape rect(static_cast<sf::Vector2f>(window.getSize()));
    rect.setPosition(0, 0);
    rect.setTexture(&image);
    rect.setFillColor(sf::Color(255, 255, 255, 70));

    assert(game_session.get_game_status() == BL::Game_status::ACTION);
    bool drag = false;
    int number_of_dragged_buttons = 0;
    std::vector<int> dragged_key(
        sf::Keyboard::KeyCount);  // чтобы различать какая кнопка зажата
    std::vector<int> dragged_mouse_button(
        sf::Mouse::ButtonCount);  // то же самое только про мышку

    sf::Event event{};

    // WARNING: lambda zone
    auto handle_click = [&]() -> void {
        drag = true;  //Зажимаю мышку
        if (field.get_field_objects().empty()) {
            return;
        }
        USO::Map_object &back_object =
            *(field.get_field_objects().back().get());
        if (!(*(field.get_field_objects().back()))
                 .check_event(
                     static_cast<sf::Vector2f>(sf::Mouse::getPosition()),
                     game_session, past_time + clock.getElapsedTime())) {
            return;
        }
        if (typeid(back_object) != typeid(USO::Aim_slider) &&
            typeid(back_object) != typeid(USO::Aim_spinner)) {
            field.get_field_objects().pop_back();
        }
        sound.play();
    };
    // lambda zone ends

    clock.restart();
    sf::CircleShape mouse(5.f);
    mouse.setFillColor(sf::Color(241, 200, 14));
    music.play();

    while (true) {
        window.draw(rect);
        game_session.table_of_scores(window, font);
        mouse.setPosition((sf::Vector2f)sf::Mouse::getPosition());
        window.draw(mouse);

        if (game_session.get_health() == 0
                && game_session.get_game_status() != BL::Game_status::NEED_TO_RETRY) {
            game_session.set_game_status(BL::Game_status::DEFEAT);
        }
        if (map_objects.back()->get_start_time() +
                map_objects.back()->get_duration_time() < past_time + clock.getElapsedTime()
                - sf::seconds(0.5)
            && game_session.get_game_status() != BL::Game_status::NEED_TO_RETRY) {
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
                        return;
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
                        if (!dragged_key[event.key.code]) {
                            dragged_key[event.key.code] = 1;
                            number_of_dragged_buttons++;
                        }
                        handle_click();
                        break;
                    case sf::Event::MouseButtonPressed:
                        if (event.mouseButton.button != sf::Mouse::Left &&
                            event.mouseButton.button != sf::Mouse::Right) {
                            break;
                        }
                        if (!dragged_mouse_button[event.mouseButton.button]) {
                            number_of_dragged_buttons++;
                            dragged_mouse_button[event.mouseButton.button] = 1;
                        }
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

                        dragged_key[event.key.code] = 0;
                        number_of_dragged_buttons--;
                        if (number_of_dragged_buttons == 0) {
                            drag = false;
                        }
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
                        number_of_dragged_buttons--;
                        if (number_of_dragged_buttons == 0) {
                            drag = false;
                        }
                        break;
                    default:
                        if (drag && !field.get_field_objects().empty()) {
                            USO::Map_object &back_object =
                                *(field.get_field_objects().back().get());
                            if (typeid(back_object) !=
                                typeid(USO::Aim_slider) &&
                                typeid(back_object) != typeid(USO::Aim_spinner)) {
                                break;
                            }
                            back_object.check_event(
                                sf::Vector2f(sf::Mouse::getPosition()),
                                game_session,
                                past_time + clock.getElapsedTime());
                        }
                        break;
                }
                break;
            }

            case BL::Game_status::PAUSE: {
                music.pause();
                past_time += clock.getElapsedTime();
                mouse.setPosition((sf::Vector2f)sf::Mouse::getPosition());
                window.draw(mouse);
                Menu::stop_menu(window, game_session);
                clock.restart();
                if (game_session.get_game_status() == BL::Game_status::NEED_TO_RETRY) {
                    past_time -= past_time;
                    break;
                }
                if (game_session.get_game_status() == BL::Game_status::ACTION) {
                    music.play();
                } else {
                    return;
                }
            } break;
            case BL::Game_status::VICTORY: {
                while (true) {
                    if (window.pollEvent(event)) {
                        if (event.key.code == sf::Keyboard::Space) {
                            field.get_field_objects().clear();
                            mouse.setPosition((sf::Vector2f)sf::Mouse::getPosition());
                            window.draw(mouse);
                            Menu::stop_menu(window, game_session);
                            if (game_session.get_game_status() == BL::Game_status::NEED_TO_RETRY) {
                                break;
                            }
                            return;
                        }
                    }
                    window.draw(rect);
                    game_session.table_of_scores(window, font);
                    mouse.setPosition((sf::Vector2f)sf::Mouse::getPosition());
                    window.draw(mouse);
                    window.display();
                }
            } break;
            case BL::Game_status::DEFEAT: {
                music.stop();
                field.get_field_objects().clear();
                mouse.setPosition((sf::Vector2f)sf::Mouse::getPosition());
                window.draw(mouse);
                Menu::stop_menu(window, game_session);
                if (game_session.get_game_status() == BL::Game_status::NEED_TO_RETRY) {
                    break;
                }
                return;
            }
            case BL::Game_status::NEED_TO_RETRY: {
                game_session.set_health(MAX_HEALTH);
                game_session.set_combo(1);
                game_session.nullify_score();
                music.stop();
                field.get_field_objects().clear();
                game_session.set_game_status(BL::Game_status::ACTION);
                current_object_it = map_objects.begin();
                music.play();
                clock.restart();
            } break;
            default: {
                continue;
            }
        }
    }
}
