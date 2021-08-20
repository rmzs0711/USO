#include "menu_objects.h"
#include "main.h"
bool Menu::Button::is_circle_correct_click(const sf::Vector2f &mouse) {
    auto pos = circle.getPosition();
    auto radius = circle.getRadius();
    return (mouse.x - pos.x - radius) * (mouse.x - pos.x - radius) +
           (mouse.y - pos.y - radius) * (mouse.y - pos.y - radius) <=
           radius * radius;
}

void Menu::Button::guidance(const sf::Vector2f &mouse) {
    if (is_circle_correct_click(mouse)) {
        state = State::POINTED;
    }
}

Action Menu::Button::press(sf::RenderWindow &window,
                         const sf::Vector2f &mouse,
                         BL::Game_session &gameSession) {
    if (is_circle_correct_click(mouse)) {
        switch (event) {
            case EXIT: {
                return menuObject.action = Action::CLOSE_THE_WINDOW;
            } break;
            case RETRY: {
                //gameSession.set_game_status(BL::Game_status::NEED_TO_RETRY);
                return menuObject.action = Action::OPEN_GAME_MAP;
            } break;
            case CHOOSE_THE_MAP: {
//                Menu::scrolling_menu scrollingMenu(R"(data\maps\saved_maps.txt)");
//                scrollingMenu.run(window);
                return menuObject.action = Action::OPEN_SCROLLING_MENU;
            } break;
            case CREATE_NEW_MAP: {
//                Menu::map_creation_menu mapCreationMenu(R"(data\maps\saved_maps.txt)");
//                mapCreationMenu.draw(window);
                return menuObject.action = Action::OPEN_CREATION_MENU;
            } break;
            case CONTINUE: {
                gameSession.set_game_status(BL::Game_status::ACTION);
                return Action::NOTHING;
            } break;
            case BACK_TO_MENU: {
//                mainMenu.run(window, gameSession);
                return menuObject.action = Action::OPEN_MAIN_MENU;
            } break;
            case OPEN_LIST_OF_MODS: {
//                mod_menu modMenu;
//                modMenu.draw(window);   /// TODO
            } break;
        }
    } else {
        return menuObject.action = Action::NOTHING;
    }
}

void Menu::Button::draw(sf::RenderWindow &window) {
    auto prev_radius = circle.getRadius();
    if (state == State::POINTED) {
        circle.setRadius(prev_radius + 20);
        state = State::QUIET;
    }
    window.draw(circle);
    circle.setRadius(prev_radius);

    std::string name_of_button;
    switch (event) {
        case EXIT:
            name_of_button = "Exit";
            break;
        case CHOOSE_THE_MAP:
            name_of_button = "Choose the map";
            break;
        case CREATE_NEW_MAP:
            name_of_button = "Create new map";
            break;
        case RETRY:
            name_of_button = "Retry";
            break;
        case CONTINUE:
            name_of_button = "Continue";
            break;
        case BACK_TO_MENU:
            name_of_button = "Back to menu";
            break;
        case OPEN_LIST_OF_MODS: {
            name_of_button  = "Mods";
        } break;
    }

    sf::Font font;
    font.loadFromFile(R"(data\fonts\aller.ttf)");
    text.setFont(font);
    text.setStyle(sf::Text::Bold);
    text.setString(name_of_button);
    text.setPosition(circle.getPosition().x + circle.getRadius() - float(name_of_button.size() * text.getCharacterSize()) / 4,
                     circle.getPosition().y + circle.getRadius() - float(text.getCharacterSize()) / 2);
    text.setFillColor(sf::Color::Black);
    window.draw(text);
}

void Menu::Button::changeSize(float coef, float win_size_x) {
    auto radius = circle.getRadius();
    circle.setRadius(radius + radius * coef);
    text.setCharacterSize(int(win_size_x / 50.f));
}
void Menu::Button::changePosition(float coef, sf::Vector2f WINDOW_POSITION, sf::Vector2f tmp) {
    sf::Vector2f pos = circle.getPosition() - tmp;
    pos += sf::Vector2f(pos.x * coef, pos.y * coef);
    circle.setPosition(pos + WINDOW_POSITION);
}
