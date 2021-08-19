#include "maps.h"
#include "loading_menu.h"
#include "main.h"

menu_object menuObject(5);
int a;

int main() {
    //menu_object menuObject;
    //menuObject.run(Menu::set_settings());
    a = 1;
    //menuObject(5);
    //Menu::set_settings();
    BL::Game_session gameSession;
    //Menu::stop_menu stopMenu(BL::Game_status::VICTORY);
    Menu::main_menu mainMenu;
    mainMenu.run(Menu::set_settings(), gameSession);
    return 0;
}