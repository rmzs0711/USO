#include "maps.h"
#include "menu.h"

int main() {
    /*sf::ContextSettings setting;
    setting.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(1080, 720), "USO!",
                            sf::Style::Fullscreen, setting);
    window.setVerticalSyncEnabled(true);
    window.display();

//    USO::Aim_map f("ff", "Rama", R"(data\music\stronger.ogg)", "Gold",
//                   R"(data\img\lucifer.png)", R"(data\fonts\mono.otf)",
//                   R"(data\music\click_sound.ogg)");
//    f.constructor_run(window);

    USO::Aim_map f(R"(data/maps/demo_gold_rush.txt)");
    f.run(window);*/
    // BL::Game_session f;
    Menu::menu(Menu::set_settings(), {});
    return 0;
}