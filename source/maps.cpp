//
// Created by Aigerim on 18.02.2021.
//
#include "maps.h"
#include <fstream>
#include <iostream>

USO::Aim_map::Aim_map(const std::string& filename) : Map() {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "File not found\n";
        return ;
    }
    //видимо тут потом разделить на режимы
    file >> map_name;
    file >> music_address;
    file >> author_name;
    file >> music_name;
    while (!file.eof()) {
        std::string type;
        file >> type;
        int index;
        file >> index;
        float time;
        file >> time;
        sf::Time start_time = sf::seconds(time);
        file >> time;
        sf::Time duration_time = sf::seconds(time);
        float x_pos;
        file >> x_pos;
        float y_pos;
        file >> y_pos;

        if (type == "Aim_circle") {
            float beat_radius;
            file >> beat_radius;
            float active_circle_radius;
            file >> active_circle_radius;
            float active_circle_shift; // может константа?
            file >> active_circle_shift;

            map_objects.push_back(std::make_shared<USO::Aim_circle>
                (USO::Aim_circle(start_time,
                                   duration_time,
                                   x_pos,
                                   y_pos,
                                   index,
                                   beat_radius,
                                   active_circle_radius,
                                   active_circle_shift)));

        } else if (type == "Aim_slider") {
            float beat_radius;
            file >> beat_radius;
            float active_circle_radius;
            file >> active_circle_radius;
            float active_circle_shift; // может константа?
            file >> active_circle_shift;
            float x_shift;
            file >> x_shift;
            float y_shift;
            file >> y_shift;
            float x_end;
            file >> x_end;
            float y_end;
            file >> y_end;

            map_objects.push_back(std::make_shared<USO::Aim_circle>
                (USO::Aim_slider(start_time,
                              duration_time,
                                 x_pos,
                                 y_pos,
                                 index,
                                 beat_radius,
                                 active_circle_radius,
                                 active_circle_shift,
                                 x_shift,
                                 y_shift,
                                 x_end,
                                 y_end)));

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