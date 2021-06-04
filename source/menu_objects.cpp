#include "menu_objects.h"
#include "menu.h"

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

void Menu::Button::press(sf::RenderWindow &window,
                         const sf::Vector2f &mouse,
                         BL::Game_session &gameSession) {
    if (is_circle_correct_click(mouse)) {
        switch (event) {
            case EXIT: {
                window.close();
            } break;
            case RETRY: {
                gameSession.set_game_status(BL::Game_status::NEED_TO_RETRY);
            } break;
            case OPEN_AIM: {
                Menu::scrolling_menu scrollingMenu(
                    R"(data\maps\saved_maps.txt)");
                scrollingMenu.draw(window);
            } break;
            case OPEN_CONVEYOR: {
                USO::Conveyor_map test(R"(input.txt)");
                test.run(window);
            } break;
            case CONTINUE: {
                gameSession.set_game_status(BL::Game_status::ACTION);
            } break;
            case BACK_TO_MENU: {
                menu(window, gameSession);
            } break;
        }
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
        case OPEN_AIM:
            name_of_button = "Aim mode";
            break;
        case OPEN_CONVEYOR:
            name_of_button = "Conveyor mode";
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
    }

    sf::Text text;
    sf::Font font;
    font.loadFromFile(R"(data\fonts\aller.ttf)");
    text.setFont(font);
    text.setCharacterSize(40);
    text.setStyle(sf::Text::Bold);
    text.setString(name_of_button);
    text.setPosition(circle.getPosition().x + circle.getRadius() - name_of_button.size() * 10,
                     circle.getPosition().y + circle.getRadius() - 40);
    text.setFillColor(sf::Color::Black);
    window.draw(text);
}
