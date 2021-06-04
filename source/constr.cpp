#include "maps.h"
#include "menu.h"

int main() {
    sf::ContextSettings setting;
    setting.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(1080, 720), "USO!",
                            sf::Style::Fullscreen, setting);
    window.setVerticalSyncEnabled(true);
    window.display();

        USO::Conveyor_map f("Conveyor",
                        "ff",
                        "input.txt",
                        R"(data\music\stronger.ogg)", "Gold",
                       R"(data\img\stronger.png)", R"(data\fonts\aller.ttf)",
                       R"(data\music\click_sound.ogg)");
//    USO::Aim_map f(R"(data/maps/stronger.txt)");
//        USO::Conveyor_map f(R"(input.txt)");
    f.constructor_run(window);

    //    USO::Aim_map f(R"(data/maps/editing_map.txt)");
    //    f.run(window);
    return 0;
}