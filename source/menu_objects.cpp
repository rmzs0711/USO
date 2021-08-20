#include "menu_objects.h"
#include "menu.h"
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

bool Menu::Button::press(sf::RenderWindow &window,
                         const sf::Vector2f &mouse,
                         BL::Game_session &gameSession) {
    if (is_circle_correct_click(mouse)) {
        switch (event) {
            case EXIT: {
                menuObject.action = Action::CLOSE_THE_WINDOW;
                return true;
            } break;
            case RETRY: {
                menuObject.game.nullify();
                menuObject.action = Action::OPEN_GAME_MAP;
                return true;
            } break;
            case CHOOSE_THE_MAP: {
                menuObject.action = Action::OPEN_SCROLLING_MENU;
                return true;
            } break;
            case CREATE_NEW_MAP: {
                menuObject.action = Action::OPEN_CREATION_MENU;
                return true;
            } break;
            case CONTINUE: {
                menuObject.action = OPEN_GAME_MAP;
                return true;
            } break;
            case OPEN_CONSTR:
            case BACK_TO_MENU: {
                menuObject.game.nullify();
                menuObject.action = Action::OPEN_MAIN_MENU;
                return true;
            }
            case OPEN_LIST_OF_MODS: {
//                mod_menu modMenu;
//                modMenu.draw(window);   /// TODO
            } break;
        }
    }
    menuObject.action = Action::NOTHING;
    return false;
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
        case OPEN_CONSTR: {
            name_of_button = "Open constructor";
        }
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
