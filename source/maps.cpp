//
// Created by Aigerim on 18.02.2021.
//
#include "maps.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include "SFML/System/Time.hpp"

USO::Aim_map::Aim_map(const std::string &filename) : Map() {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "File not found\n";
        std::exit(3);
    }
    file >> map_name;
    file >> author_name;
    file >> music_address;
    file >> music_name;
    file >> image_address;
    file >> font_address;
    file >> sound_address;

    while (!file.eof()) {
        std::string type;
        file >> type;
//        int index;
//        file >> index;
        sf::Int64 time;
        file >> time;
        sf::Time start_time = sf::microseconds(time);
        file >> time;
        sf::Time duration_time = sf::microseconds(time);
        float x_pos;
        file >> x_pos;
        float y_pos;
        file >> y_pos;

        if (type == "Aim_circle") {
            float beat_radius;
            file >> beat_radius;
            float active_circle_radius;
            file >> active_circle_radius;

            map_objects.push_back(std::make_shared<USO::Aim_circle>(
                USO::Aim_circle(start_time, duration_time, x_pos, y_pos,
                                beat_radius, active_circle_radius)));

        } else if (type == "Aim_slider") {
            float beat_radius;
            file >> beat_radius;
            float active_circle_radius;
            file >> active_circle_radius;
            float x_end;
            file >> x_end;
            float y_end;
            file >> y_end;
            int move_time;
            file >> move_time;

            map_objects.push_back(std::make_shared<USO::Aim_slider>(
                USO::Aim_slider(start_time, duration_time, x_pos, y_pos,
                                beat_radius, active_circle_radius, x_end, y_end,
                                sf::microseconds(move_time))));

        } /*else if (type == "Aim_muda") {
            float beat_radius;
            file >> beat_radius;
            float beat_count;
            file >> beat_count;
            map_objects.push_back(std::make_shared<USO::Aim_muda>
                 (USO::Aim_muda(start_time,
                                duration_time,
                                x_pos,
                                y_pos,
                                index,
                                beat_radius,
                                beat_count)));
        //НЕ СМОГЛА ПОТОМУ ЧТО АБСТРАКТНЫЙ КЛАСС
        }*/
    }
}

USO::Conveyor_map::Conveyor_map(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "File not found\n";
        return;
    }
    file >> map_name;
    file >> music_address;
    file >> author_name;
    file >> music_name;

    float pos_x;
    file >> pos_x;
    float pos_y;
    file >> pos_y;
    float width;
    file >> width;
    float height;
    file >> height;

    lines.push_back(std::make_shared<USO::Conveyor_line>(USO::Conveyor_line(
        sf::Vector2f(pos_x, pos_y), sf::Vector2f(width, height), 0)));

    for (int i = 1; i < NUMBER_OF_LINES; ++i) {
        lines.push_back(std::make_shared<USO::Conveyor_line>(
            USO::Conveyor_line(sf::Vector2f(pos_x + (float)i * width, pos_y),
                               sf::Vector2f(width, height), i)));
    }

    while (!file.eof()) {
        int index;
        file >> index;
        int32_t time;
        file >> time;
        sf::Time start_time = sf::milliseconds(time);
        file >> time;
        sf::Time duration_time = sf::milliseconds(time);
        int line_index;
        file >> line_index;

        map_objects.push_back(
            std::make_shared<USO::Conveyor_note>(USO::Conveyor_note(
                start_time, duration_time, index, *(lines[line_index]))));
    }
}
void USO::Map::prelude(sf::Music &music,
                       sf::SoundBuffer &sound_buffer,
                       sf::Texture &image,
                       sf::Font &font) {
    check_file_load(music.openFromFile(music_address), music_address);
    check_file_load(sound_buffer.loadFromFile(sound_address), sound_address);
    check_file_load(image.loadFromFile(image_address), image_address);
    check_file_load(font.loadFromFile(font_address), font_address);
}
