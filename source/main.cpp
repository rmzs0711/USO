//#define _GLIBCXX_USE_CXX11_ABI 0
#include "SFML/Audio.hpp"
#include "SFML/Graphics.hpp"
#include "base_logic.h"
#include "map_master.h"
#include "map_objects.h"
#include "maps.h"
#include "windows.h"
#include <cassert>
#include <random>
// using namespace sf;

int main()
{
    sf::ContextSettings setting;

    setting.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(1080, 720), "SFML Works!", sf::Style::Fullscreen, setting);
    window.setVerticalSyncEnabled(true);

    USO::Aim_map test(R"(data\maps\test_map_longer.txt)");
    int h = 0;
//    for (auto &i : test.map_objects)
//    {
//        const auto &g = *i;
//        sf::Int32 t = g.duration_time.asMilliseconds();
//        h++;
//    }

    test.run(window);

    return 0;
}