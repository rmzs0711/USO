#include <cassert>
#include <functional>
#include <iostream>
#include "SFML/Graphics.hpp"
#include "base_logic.h"
#include "map_management.h"
#include "maps.h"

namespace {}  // namespace

void USO::Conveyor_map::run(sf::RenderWindow &window) {
    BL::Game_session game_session;
    USO::Field field(window, {});
    sf::Clock clock;
    sf::SoundBuffer press_sound;
    sf::Time past_time;  // костыль для паузы, так как sfml не умеет
    // останавливать часы
    auto current_object_it =
        map_objects.begin();  // итератор на следующий по времени объект
    sf::Music music;

    check_file_load(press_sound.loadFromFile(R"(data/music/click_sound.ogg)"),
                    R"(data/music/click_sound.ogg)");
    music.openFromFile(music_address);
    music.play();

    sf::Sound sound;
    sound.setBuffer(press_sound);

    sf::Texture img;
    check_file_load(
        img.loadFromFile(R"(data\img\lucifer.png)"),
        R"(data\img\lucifer.png)");  //Тут нужно сделать загрузку названия из
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
    std::vector<int> note_keys = {sf::Keyboard::D, sf::Keyboard::F,
                                  sf::Keyboard::J, sf::Keyboard::K};

    clock.restart();
    int number_of_dragged_buttons = 0;
    while (game_session.get_game_status() != BL::Game_status::VICTORY ||
           game_session.get_game_status() != BL::Game_status::DEFEAT) {
        window.draw(rect);
        game_session.table_of_scores(window, font);
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

                sf::Event event{};
                for (auto &line : lines) {
                    line->draw(window);
                }
                field.draw(font);

                window.display();

                if (!window.pollEvent(event) && !drag) {
                    continue;
                }

                /// WARNING: lambda zone
                auto handle_click = [&]() -> void {
                    drag = true;  //Зажимаю мышку
                    for (int i = 0; i < NUMBER_OF_LINES; ++i) {
                        if (dragged_key[note_keys[i]]) {
                            lines[i]->dragged = true;
                        }
                    }
                    if (field.get_field_objects().empty()) {
                        return;
                    }
                    USO::Map_object &front_object =
                        *(field.get_field_objects().back().get());

                    for (int i = 0; i < NUMBER_OF_LINES; ++i) {
                        sf::Vector2f position = lines[i]->beat_pos;
                        if (dragged_key[note_keys[i]]) {
                            lines[i]->dragged = true;
                            for (auto &note : field.get_field_objects()) {
                                if (note->check_event(
                                        position, game_session,
                                        past_time + clock.getElapsedTime())) {
                                    sound.play();
                                }  // иначе - снять хп
                            }
                        }
                    }
                };
                // lambda zone ends

                switch (event.type) {
                    case sf::Event::KeyPressed:
                        if (event.key.code == sf::Keyboard::Escape) {
                            // остановить время на паузу
                            return;
                        }
                        if (!dragged_key[event.key.code]) {
                            dragged_key[event.key.code] = 1;
                            number_of_dragged_buttons++;
                        }
                        handle_click();
                        break;
                    case sf::Event::KeyReleased:

                        dragged_key[event.key.code] = 0;
                        number_of_dragged_buttons--;
                        for (int i = 0; i < NUMBER_OF_LINES; ++i) {
                            if (!dragged_key[note_keys[i]]) {
                                lines[i]->dragged = false;
                            }
                        }
                        if (number_of_dragged_buttons == 0) {
                            drag = false;
                        }
                        break;
                    default:
                        /*
                        if (drag && !field.get_field_objects().empty()) {
                            USO::Map_object &front_object =
                                *(field.get_field_objects().front().get());
                            //                            if
                            //                            (typeid(front_object)
                            //                            !=
                            // typeid(USO::Aim_slider))
                            //                                { break;
                            //                            }
                            front_object.check_event(
                                sf::Vector2f(sf::Mouse::getPosition()),
                                game_session,
                                past_time + clock.getElapsedTime());
                        }*/
                        break;
                }
                break;
            }

            case BL::Game_status::PAUSE: {
                //когда отожмут паузу, нужно сделать restart
                // clock, потому что
                // sfml по пацански не останавливается
                break;
            }
            case BL::Game_status::DEFEAT: {
                return;
            }
            default:
                continue;
        }
    }
}
