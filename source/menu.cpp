//
// Created by Aigerim on 15.04.2021.
//

#include "menu.h"
#include <fstream>
#include <utility>
#include <vector>
#include "maps.h"
#include "menu_objects.h"
#include <string>

namespace {
std::string list_of_saved_maps_file_name() {
    static std::string saved_maps = R"(data\saved_maps)";
    return saved_maps;
}
sf::Vector2f get_constructor_menu_buttons_size() {
    static sf::Vector2f sizes(500, 200);
    return sizes;
}
std::vector<std::string> get_vector_of_saved_maps_names() {
    std::vector<std::string> res;
    std::ifstream input(list_of_saved_maps_file_name());
    for (std::string line; !input.eof();) {
        std::getline(input, line);
        res.push_back(line);
    }
    return res;
}
}  // namespace

sf::RenderWindow &Menu::set_settings() {
    sf::ContextSettings setting;
    setting.antialiasingLevel = 8;
    static sf::RenderWindow window(sf::VideoMode(1080, 720), "USO!",
                            sf::Style::Fullscreen, setting);
    window.setMouseCursorVisible(false);
    window.setVerticalSyncEnabled(true);
    window.display();
    return window;
}

void Menu::menu(sf::RenderWindow &window, BL::Game_session gameSession) {
    std::vector<Menu::Button> buttons;
    buttons.emplace_back(400, 400, 200, Menu::OPEN_AIM);
    buttons.emplace_back(0, 0, 100, Menu::EXIT, sf::Color::Cyan);
    buttons.emplace_back(700, 50, 100, Menu::OPEN_SETTINGS,
                         sf::Color::Green);
    buttons.emplace_back(900, 400, 200, Menu::OPEN_CONVEYOR,
                         sf::Color::Magenta);

    sf::CircleShape mouse(5.f);

    while (window.isOpen()) {
        window.clear();
        sf::Event event{};
        if (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::KeyPressed: {
                    if (event.key.code == sf::Keyboard::Escape) {
                        window.close();
                        return;
                    }
                } break;
                case sf::Event::MouseButtonPressed: {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        for (auto &button : buttons) {
                            button.press(
                                window,
                                {static_cast<float>(event.mouseButton.x),
                                 static_cast<float>(event.mouseButton.y)}, gameSession);
                        }
                    }
                } break;
                case sf::Event::Closed: {
                    window.close();
                    return;
                }
                case sf::Event::MouseMoved: {
                    for (auto &button : buttons) {
                        button.guidance(
                            {static_cast<float>(event.mouseMove.x),
                             static_cast<float>(event.mouseButton.y)});
                    }
                } break;
                default: {
                } break;
            }
        }
        for (auto &button : buttons) {
            button.guidance((sf::Vector2f)sf::Mouse::getPosition());
            button.draw(window);
            mouse.setPosition((sf::Vector2f)sf::Mouse::getPosition());
            mouse.setFillColor(sf::Color(241, 200, 14));
            window.draw(mouse);
        }
        window.display();
    }
}

void Menu::stop_menu(sf::RenderWindow &window, BL::Game_session &gameSession) {
    std::vector<Menu::Button> buttons;
    buttons.emplace_back(400, 400, 200, Menu::RETRY);
    buttons.emplace_back(0, 0, 100, Menu::BACK_TO_MENU, sf::Color::Cyan);
    if (gameSession.get_game_status() == BL::Game_status::PAUSE) {
        buttons.emplace_back(900, 400, 100, Menu::CONTINUE, sf::Color::Magenta);
    }

    sf::CircleShape mouse(5.f);
    while (window.isOpen() && (gameSession.get_game_status() == BL::Game_status::PAUSE
                            || gameSession.get_game_status() == BL::Game_status::DEFEAT
                            || gameSession.get_game_status() == BL::Game_status::VICTORY)) {
        sf::Event event{};
        window.clear();
        if (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::KeyPressed: {
                    if (event.key.code == sf::Keyboard::Escape) {
                        menu(window, gameSession);
                        return;
                    }
                } break;
                case sf::Event::MouseButtonPressed: {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        for (auto &button : buttons) {
                            button.press(window,
                                (sf::Vector2f)sf::Mouse::getPosition(), gameSession);
                        }
                    }
                } break;
                case sf::Event::Closed: {
                    window.close();
                    return;
                }
                case sf::Event::MouseMoved: {
                    for (auto &button : buttons) {
                        button.guidance(
                            {static_cast<float>(event.mouseMove.x),
                             static_cast<float>(event.mouseButton.y)});
                    }
                } break;
                default: {
                } break;
            }
        }

        for (auto &button : buttons) {
            button.guidance((sf::Vector2f)sf::Mouse::getPosition());
            button.draw(window);
            mouse.setPosition((sf::Vector2f)sf::Mouse::getPosition());
            mouse.setFillColor(sf::Color(241, 200, 14));
            window.draw(mouse);
        }
        window.display();
    }
}

void Menu::constructor_menu(sf::Window &window) {
    std::vector<std::string> saved_maps(get_vector_of_saved_maps_names());
    sf::Vector2f sizes = get_constructor_menu_buttons_size();

    std::vector<Menu::Button> buttons;

    float start_x = static_cast<float>(window.getSize().x) - sizes.x;
    float start_y = sizes.y;
    for (auto &map_name: saved_maps) {
//        buttons.emplace_back(start_x, start_y, );
    }
}

Menu::scrolling_menu::scrolling_menu(std::string filename_) : filename(std::move(filename_)) {
    std::ifstream file(filename);
    std::string map_name;
    while (std::getline(file, map_name)) {
        std::cout << map_name << "\n";
        list_of_maps.emplace_back(map_name);
    }
    construct_new_map.setSize(sf::Vector2f(600, 100));
    construct_new_map.setPosition(600, 0);
    construct_new_map.setOutlineThickness(5);
    construct_new_map.setOutlineColor(sf::Color(0, 0, 0));

    for (int i = 0; i < std::min(MAX_SIZE, list_of_maps.size()); i++) {
        blocks_of_maps_name.emplace_back(sf::Vector2f(600, 100));
        blocks_of_maps_name[i].setPosition(0, (float)i * 100);
        blocks_of_maps_name[i].setOutlineThickness(5);
        blocks_of_maps_name[i].setOutlineColor(sf::Color(0, 0, 0));
    }
    font.loadFromFile(R"(data\fonts\GistLight.otf)");
    text.setFont(font);
    text.setCharacterSize(40);
    text.setFillColor(sf::Color(34, 46, 137));
    text.setStyle(sf::Text::Bold);

    for (int i = 0; i < 8; i++) {
        blocks_of_maps_data.emplace_back(sf::Vector2f(600, 100));
        blocks_of_maps_data[i].setPosition(0, (float)i * 100);
        blocks_of_maps_data[i].setOutlineThickness(5);
        blocks_of_maps_data[i].setOutlineColor(sf::Color(0, 0, 0));
    }
    text.setFillColor(sf::Color(2, 2, 2));
    text.setScale(0.7, 0.7);
    list_of_data.resize(8);
}

bool Menu::scrolling_menu::push(sf::RenderWindow &window, sf::Vector2f mouse) {
    if (construct_new_map.getPosition().x <= mouse.x &&
        construct_new_map.getPosition().x +
        construct_new_map.getSize().x >= mouse.x &&
        construct_new_map.getPosition().y +
        construct_new_map.getSize().y >= mouse.y &&
        construct_new_map.getPosition().y <= mouse.y) {
        get_input(window);

        if (list_of_data[0] == "Aim") {
            USO::Aim_map test(list_of_data[0], list_of_data[1],
                              list_of_data[2], R"(data\music\)" + list_of_data[3] ,
                              list_of_data[4], R"(data\img\)" + list_of_data[5],
                              R"(data\fonts\)" + list_of_data[6], R"(data\sounds\)" + list_of_data[7]); // запуск конструктора карт !!!
            add_new_map(list_of_data[1]);
            test.constructor_run(window);
        } else if (list_of_data[0] == "Conveyor") {
            USO::Conveyor_map test(list_of_data[0], list_of_data[1],
                              list_of_data[2], R"(data\music\)" + list_of_data[3] ,
                              list_of_data[4], R"(data\img\)" + list_of_data[5],
                              R"(data\fonts\)" + list_of_data[6], R"(data\sounds\)" + list_of_data[7]); // запуск конструктора карт !!!
            add_new_map(list_of_data[1]);
            test.constructor_run(window);
        }
        return false;
    }
    for (int i = 0; i < blocks_of_maps_name.size(); i++) {
        if (blocks_of_maps_name[i].getPosition().x <= mouse.x &&
            blocks_of_maps_name[i].getPosition().x +
                    blocks_of_maps_name[i].getSize().x >= mouse.x &&
            blocks_of_maps_name[i].getPosition().y +
                    blocks_of_maps_name[i].getSize().y >= mouse.y &&
            blocks_of_maps_name[i].getPosition().y <= mouse.y) {
            USO::Aim_map test(R"(data\maps\)" + list_of_maps[i + delta] + ".txt");
            test.run(window);
            return false;
        }
    }
    return true;
}

int Menu::scrolling_menu::get_delta() const {
    return delta;
}

void Menu::scrolling_menu::scrolling_down() {
    if (delta + std::min(MAX_SIZE, list_of_maps.size()) < list_of_maps.size() - 1) {
        increase_delta();
    }
}

void Menu::scrolling_menu::scrolling_up() {
    if (delta != 0) {
        decrease_delta();
    }
}

void Menu::scrolling_menu::increase_delta() {
    delta++;
}

void Menu::scrolling_menu::decrease_delta() {
    delta--;
}

void Menu::scrolling_menu::draw(sf::RenderWindow &window) {
    sf::CircleShape mouse(5.f);
    while (window.isOpen()) {
        window.clear();
        sf::Event event{};
        if (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::MouseButtonPressed: {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        if (push(window, (sf::Vector2f)sf::Mouse::getPosition())) {
                            return;
                        }
                    }
                } break;
                case sf::Event::MouseWheelScrolled: {
                    if (event.mouseWheelScroll.delta < 0) {
                        scrolling_down();
                    } else if (event.mouseWheelScroll.delta > 0) {
                        scrolling_up();
                    }
                } break;
                case sf::Event::KeyPressed: {
                    if (event.key.code == sf::Keyboard::Escape) {
                        return;
                    }
                } break;
            }
        }
        text.setString("Create new map");
        text.setPosition(650, 0);
        window.draw(construct_new_map);
        window.draw(text);

        for (int i = 0; i < std::min(MAX_SIZE, list_of_maps.size()); i++) {
            window.draw(blocks_of_maps_name[i]);
            text.setString(list_of_maps[i + delta]);
            text.setPosition(0, (float)i * 100);
            window.draw(text);
            mouse.setPosition((sf::Vector2f)sf::Mouse::getPosition());
            mouse.setFillColor(sf::Color(241, 200, 14));
            window.draw(mouse);
        }
        window.display();
    }
}

void Menu::scrolling_menu::add_new_map(const std::string &map_name) {
    list_of_maps.emplace_back(map_name);
    std::ofstream file(filename);
    file << map_name;
    std::size_t size_of_table = std::min(MAX_SIZE, list_of_maps.size());
    if (blocks_of_maps_name.size() < size_of_table) {
        blocks_of_maps_name.emplace_back(sf::Vector2f(600, 100));
        blocks_of_maps_name[blocks_of_maps_name.size() - 1].setPosition(0, (float)(size_of_table - 1) * 100);
    }
}



void Menu::scrolling_menu::get_input(sf::RenderWindow &window) {
//    list_of_data[0] = "Aim";
//    list_of_data[1] = "my_map";
//    list_of_data[2] = "Sasha";
//    list_of_data[3] = "drum_go_dum.ogg";
//    list_of_data[4] = "my_music";
//    list_of_data[5] = "stronger.png";
//    list_of_data[6] = "GistLight.otf";
//    list_of_data[7] = "click.ogg";  // check !!!
    sf::CircleShape mouse(5.f);
    int index = -1;
    list_of_data.resize(8);
    while (window.isOpen()) {
        window.clear();
        sf::Event event{};
        if (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::MouseButtonPressed: {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        index = get_id((sf::Vector2f)sf::Mouse::getPosition());
                    }
                } break;
                case sf::Event::KeyPressed: {
                    if (event.key.code == sf::Keyboard::Escape) {
                        for (int i = 0; i < 8; i++) {
                            text.setFillColor(sf::Color(2, 2, 2));
                        }
                        list_of_data.assign(8, {});
                        return;
                    }
                    if (event.key.code == sf::Keyboard::Enter) {
                        index = -1;
                        break;
                    }
                    if (event.key.code == sf::Keyboard::BackSpace) {
                        if (index != -1 && !list_of_data[index].empty()) {
                            list_of_data[index].pop_back();
                        }
                    }
                } break;
                case sf::Event::TextEntered: {
                    if (index != -1 && list_of_data[index].length() <= 26) {
                        list_of_data[index].push_back((char)event.text.unicode);
                    }
                } break;
            }
        }

        for (int i = 0; i < 8; i++) {
            window.draw(blocks_of_maps_data[i]);
            if (list_of_data[i].length() != 0) {
                text.setString(list_of_data[i]);
                text.setFillColor(sf::Color(2, 2, 2));
            } else {
                text.setFillColor(sf::Color(150, 160, 145));
                if (i == 0) {
                    text.setString("Enter the name of the mode");
                } else if (i == 1) {
                    text.setString("Enter the name of the map");
                } else if (i == 2) {
                    text.setString("Enter your nickname");
                } else if (i == 3) {
                    text.setString("Enter the music address");
                } else if (i == 4) {
                    text.setString("Enter the name of the music");
                } else if (i == 5) {
                    text.setString("Enter the image address");
                } else if (i == 6) {
                    text.setString("Enter the font address");
                } else if (i == 7) {
                    text.setString("Enter the sound address");
                }
            }

            text.setPosition(10, (float)i * 100);
            window.draw(text);
            mouse.setPosition((sf::Vector2f)sf::Mouse::getPosition());
            mouse.setFillColor(sf::Color(241, 200, 14));
            window.draw(mouse);
        }
        window.display();
    }
}

int Menu::scrolling_menu::get_id(sf::Vector2f mouse) const {
    for (int i = 0; i < 8; i++) {
        if (blocks_of_maps_data[i].getPosition().x <= mouse.x &&
            blocks_of_maps_data[i].getPosition().x +
            blocks_of_maps_data[i].getSize().x >= mouse.x &&
            blocks_of_maps_data[i].getPosition().y +
            blocks_of_maps_data[i].getSize().y >= mouse.y &&
            blocks_of_maps_data[i].getPosition().y <= mouse.y) {
            return i;
        }
    }
    return -1;
}

//Menu::data::data(std::string mode_,
//                 std::string map_name_,
//                 std::string author_name_,
//                 std::string music_address_,
//                 std::string music_name_,
//                 std::string image_address_,
//                 std::string font_address_,
//                 std::string sound_address_)
//        : mode(std::move(mode_)),
//          map_name(std::move(map_name_)),
//          author_name(std::move(author_name_)),
//          music_address(std::move(music_address_)),
//          music_name(std::move(music_name_)),
//          image_address(std::move(image_address_)),
//          font_address(std::move(font_address_)),
//          sound_address(std::move(sound_address_)) {
//
//}
//void Menu::data::input() {
//
//}
