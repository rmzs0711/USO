#include <cassert>
#include <functional>
#include <iostream>
#include "SFML/Graphics.hpp"
#include "base_logic.h"
#include "map_objects_management.h"
#include "maps.h"

namespace {}  // namespace

void USO::taiko_map::run(sf::RenderWindow &window) {
    BL::Game_session game_session;
    USO::Field field(window, {});
    sf::Clock clock;
    sf::SoundBuffer press_sound;
    sf::Time past_time;
    auto current_object_it = map_objects.begin();
    sf::Music music;

    check_file_load(press_sound.loadFromFile(R"(data\music\click_sound.ogg)"),
                    R"(data\music\click_sound.ogg)");
    music.openFromFile(music_address);
    music.play();

    sf::Sound sound;
    sound.setBuffer(press_sound);

    sf::Texture img;
    check_file_load(img.loadFromFile(R"(data\img\lucifer.png)"),
                    R"(data\img\lucifer.png)");

    sf::RectangleShape rect(static_cast<sf::Vector2f>(window.getSize()));
    std::cout << "RUN: " << window.getSize().x << " " << window.getSize().y << "\n";
    rect.setPosition(0, 0);
    rect.setTexture(&img);
    rect.setFillColor(sf::Color(255, 255, 255, 40));

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

    std::vector<int> note_keys = {sf::Keyboard::D, sf::Keyboard::F,
                                  sf::Keyboard::J, sf::Keyboard::K};
    int number_of_dragged_buttons = 0;

    clock.restart();


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

                sf::Event event{};
                catchZone->draw(window);
                field.draw(font);

                if (!field.get_field_objects().empty() &&
                    !field.get_field_objects().back()->change_state(
                        past_time + clock.getElapsedTime())) {
                    field.get_field_objects().back()->draw(window, font);
                    field.get_field_objects().pop_back();
                    game_session.decrease_health(BL::Game_session::damage());
                }
                window.display();

                if (!window.pollEvent(event) && !drag) {
                    continue;
                }

                auto handle_click = [&]() -> void {
                    drag = true;
                    if (dragged_key[note_keys[0]]) {
                        catchZone->dragged = true;
                    }
                  sf::Vector2f position = catchZone->basket.getPosition();
                  if (dragged_key[note_keys[0]]) {
                      catchZone->dragged = true;
                      bool hit = false;
                      while (!field.get_field_objects().empty()) {
                          if (field.get_field_objects().back()->check_event(
                                  position, game_session,
                                  past_time + clock.getElapsedTime())) {
                              sound.play();
                              field.get_field_objects().pop_back();
                              hit = true;
                              catchZone->missed = false;
                          } else {
                              break;
                          }
                      }
                      if (!hit) {
                          catchZone->missed = true;
                      }
                  }
                };

                switch (event.type) {
                    case sf::Event::KeyPressed: {
                        if (event.key.code == sf::Keyboard::Escape) {
                            return;
                        }
                        if (!dragged_key[event.key.code]) {
                            dragged_key[event.key.code] = 1;
                            number_of_dragged_buttons++;
                        }
                        handle_click();
                    } break;
                    case sf::Event::KeyReleased: {
                        dragged_key[event.key.code] = 0;
                        number_of_dragged_buttons--;
                        if (!dragged_key[note_keys[0]]) {
                            catchZone->dragged = false;
                            catchZone->missed = false;
                        }
                        if (number_of_dragged_buttons == 0) {
                            drag = false;
                        }
                    } break;
                    default: {
                        if (drag && !field.get_field_objects().empty()) {
                            USO::Map_object &front_object =
                                *(field.get_field_objects().front().get());
                            front_object.check_event(
                                sf::Vector2f(sf::Mouse::getPosition()),
                                game_session,
                                past_time + clock.getElapsedTime());
                        }
                    } break;
                }
                break;
            }
            case BL::Game_status::PAUSE: {
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