#include "maps.h"

int main() {
    sf::ContextSettings setting;
    setting.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(1080, 720), "USO!",
                            sf::Style::Fullscreen, setting);
    window.setVerticalSyncEnabled(true);
    window.display();
    USO::Aim_map f(R"(data/maps/mayday2.txt)");
    f.constructor_run(window);

    return 0;
}