/* Note: молодцы, что соблюдаете порядок в инклудах! :) Но я бы сишные импорты
 * поднял в начало. */
#include "SFML/Audio.hpp"
#include "SFML/Graphics.hpp"
#include "base_logic.h"
#include "map_master.h"
#include "map_objects.h"
#include "maps.h"
#include "windows.h"
#include <cassert>
#include <random>

int main()
{
    sf::ContextSettings setting;

    setting.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(1080, 720), "SFML Works!", sf::Style::Fullscreen, setting);
    window.setVerticalSyncEnabled(true);

    USO::Aim_map test(R"(data\maps\demo_drum_go_dum.txt)");

    test.run(window);

    return 0;
}
