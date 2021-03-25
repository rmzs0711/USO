#include <cassert>
#include <fstream>
#include <iostream>
#include <typeindex>
#include "SFML/Graphics.hpp"
#include "base_logic.h"
#include "iterator"
#include "map_master.h"
#include "maps.h"

void table_of_scores(sf::RenderWindow &window, sf::Font &font, BL::Game_session &game_session) {
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
    /* Кажется, тут подразумевается, что position = (0, 0). Лучше явно сделать setPosition. */
    /* Алсо, возможно, стоит начальные координаты вынести в параметры функции.
     * Вдруг захочется нарисовать таблицу в другом месте? */
    text.setString("SCORE: " + std::to_string(game_session.get_score()));
    window.draw(text);
    text.setPosition(sf::Vector2f(0.f, 35.f));
    text.setString("COMBO: " + std::to_string(game_session.get_combo()));
    window.draw(text);
    text.setPosition(sf::Vector2f(0.f, 70.f));
    text.setString("HEALTH: " + std::to_string(game_session.get_health()));
    window.draw(text);

}

void USO::Aim_map::run(sf::RenderWindow &window) {
    /* width и height можно получить из window (см. документацию, там есть метод
     * getSize или что-то вроде). Так оставлять опасно, ибо ничто не гарантирует,
     * что эти значения верны. */
    const unsigned HEIGHT = sf::VideoMode::getFullscreenModes().front().height;
    const unsigned WIDTH = sf::VideoMode::getFullscreenModes().front().width;
    /* Note: не все комментарии одинаково полезны, те, которые не дают новой
     * информации, скорее всего, не нужны. */
    BL::Game_session game_session;  //счетчики, статус игры
    USO::Field field;               // хранилище объектов на карте
    sf::Clock clock;                //таймер
    /* Note: если назвать переменную press_sound, то комментарий не понадобится. :)
     * Алсо, старайтесь называть переменные в одинаковом стиле и не мешать camelCase
     * со snake_case в одинаковых сущностях. Консистентно стилизованный код проще
     * и приятнее читать. */
    sf::SoundBuffer soundBuffer;    //звук нажатия
    sf::Time past_time;  // костыль для паузы, так как sfml не умеет останавливать часы
    auto current_object_it = map_objects.begin();  // итератор на следующий по времени объект
    /* Note: слишком абстрактное название переменной, bg_music или track было
     * бы более информативно */
    sf::Music buffer;                              //балдежный музон

    /* Note: для дебага норм, но вообще лучше пользователю сказать, что файла не хватает. */
    assert(soundBuffer.loadFromFile(R"(data\music\click_sound.ogg)"));
    buffer.openFromFile(music_address);
//    buffer.setVolume(100);
    buffer.play();
    sf::Sound sound;

    sound.setBuffer(soundBuffer);
    /* Вообще, оставлять закомментированный код - это моветон. В 9 из 10 случаев,
     * он никогда не будет раскомментирован. */
//    sound.setVolume(100);
    //    sf::Texture img;
    //    img.loadFromFile(R"(data\img\lucifer.png)");

    //    sf::RectangleShape rect(sf::Vector2f((float)WIDTH, (float)HEIGHT));
    //    rect.setPosition(0, 0);
    //    rect.setTexture(&img);

    sf::Font font;
    /* assert(font.loadFromFile(...))? */
    if (!font.loadFromFile(R"(data\fonts\GistLight.otf)")) {
        assert(false);
    }

    assert(game_session.get_game_status() == BL::Game_status::ACTION);
    bool drag = false;
    sf::Int32 remembered_time;
    int i = 0;
    clock.restart();
    while (game_session.get_game_status() != BL::Game_status::VICTORY ||
           game_session.get_game_status() != BL::Game_status::DEFEAT) {
        window.clear();
        //        window.draw(rect);
        table_of_scores(window, font, game_session);
        switch (game_session.get_game_status()) {
            case BL::Game_status::ACTION: {
                if (current_object_it != map_objects.end()) {
                    /* note: для дебага пойдёт, но вообще, если карты будут задаваться
                     * пользователем и будет техническая возможность сделать так,
                     * чтобы этот ассерт триггернулся, лучше поступить более робастно
                     * и просто ругнуться в stderr и пропустить этот объект. */
                    assert(*current_object_it);
                    field.push(current_object_it, past_time + clock.getElapsedTime());
                }
                for (std::shared_ptr<Map_object> &it : field.get_field_objects()) {
                    const Map_object &temp = *it;
                    (*it).change_state(past_time + clock.getElapsedTime());
                }
                if (!field.get_field_objects().empty() &&
                    !field.get_field_objects().back()->change_state(past_time + clock.getElapsedTime())) {
                    field.get_field_objects().pop_back();
                }
                sf::Event event{};
                /* Note: чтобы уменьшить вложенность кода, следующий if можно заменить
                 * на if (!window.pollEvent(event) && !drag) break;
                 * А если вынести обработку ввода в отдельную функцию, то подобным
                 * образом можно ещё красивее сделать. */
                if (window.pollEvent(event) || drag) {
                    if (event.type == sf::Event::KeyPressed) {
                        if (event.key.code == sf::Keyboard::Escape) {
                            return;
                            // past_time =
                            // clock.getElapsedTime();
                            // game_session.pause_session();
                        } else if (event.key.code == sf::Keyboard::X || event.key.code == sf::Keyboard::Z) {
                            /* Лучше вместо этого кейса оставить комментарий с TODO.
                             * Компилятор, конечно, простит, но читать будет легче. */
                        }
                    } else if (event.type == sf::Event::MouseButtonPressed) {
                        if (event.mouseButton.button == sf::Mouse::Left) {
                            if (!field.get_field_objects().empty()) {
                                drag = true;  //Зажимаю мышку
                                USO::Map_object &front_object = *(field.get_field_objects().front().get());
                                if ((*(field.get_field_objects().back()))
                                        .check_event({(float)event.mouseButton.x, (float)event.mouseButton.y},
                                                     game_session, past_time + clock.getElapsedTime())) {
                                    if (typeid(front_object) != typeid(USO::Aim_slider)) {
                                        field.get_field_objects().pop_back();
                                    }
                                    sound.play();
                                }
                            }
                        }
                    } else if (drag && event.type == sf::Event::MouseButtonReleased) {
                        if (event.mouseButton.button == sf::Mouse::Left) {
                            drag = false;  //отпускаю мышку
                        }
                    } else if (drag) {  // пока мышка нажата
                        if (!field.get_field_objects().empty()) {
                            USO::Map_object &front_object = *(field.get_field_objects().front().get());
                            if (typeid(front_object) == typeid(USO::Aim_slider)) {
                                front_object.check_event(sf::Vector2f(sf::Mouse::getPosition()), game_session,
                                                         past_time + clock.getElapsedTime());
                            }
                        }
                    }
                }

                break;
            }

            case BL::Game_status::PAUSE: {
                //когда отожмут паузу, нужно сделать restart
                // clock, потому что
                // sfml по пацански не останавливается
                break;
            }

            default:
                continue;
        }
        for (auto &object_ptr : field.get_field_objects()) {
            (*object_ptr).draw(window, font);
        }
        window.display();
    }
}
