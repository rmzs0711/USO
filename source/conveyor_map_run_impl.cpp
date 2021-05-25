//
// Created by Aigerim on 14.05.2021.
//

#include "maps.h"
#include "map_objects.h"
#include "map_management.h"

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

    check_file_load(press_sound.loadFromFile(R"(data\music\click_sound.ogg)"),
                    R"(data\music\click_sound.ogg)");
    music.openFromFile(music_address);
    music.play();
    std::cout << "it's overrided" << std::endl;
}
