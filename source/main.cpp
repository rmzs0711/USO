#include "maps.h"
#include "menu.h"

int main() {
    Menu::main_menu mainMenu;
    mainMenu.run(Menu::set_settings());
    return 0;
}