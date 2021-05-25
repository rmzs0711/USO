#include "menu.h"
#include "maps.h"

using namespace  USO;

int main() {
    Conveyor_map test("input.txt");
    sf::ContextSettings setting;
    setting.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(1080, 720), "USO!",
                            sf::Style::Fullscreen, setting);
    window.setVerticalSyncEnabled(true);
    window.display();

    while (window.isOpen()) {
        window.clear();
        for (auto &x : test.lines) {
            std::cout << x->pos.x << " " << x->pos.y << std::endl;
            std::cout << x->beat_pos.x << " " << x->beat_pos.y << std::endl;
            x->draw(window);
        }
        window.display();
    }
    return 0;
}