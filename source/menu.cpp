#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>

#include "menu.h"
#include "main.h"
#include "menu_objects.h"
#include "base_logic.h"
#include "maps.h"

std::string new_map_name;

namespace {

const sf::Color color_for_mouse(241, 200, 14);
const sf::Color input_color(150, 160, 145);
const sf::Color text_color(2, 2, 2);

float acceleration_factor = 1;
int transparent_lvl = 0;

void background_draw(sf::RenderWindow &window) {
    static sf::Texture img;
    img.loadFromFile(R"(data\img\back.png)");
    sf::RectangleShape rect(static_cast<sf::Vector2f>(menuObject.WINDOW_SIZE));
    rect.setTexture(&img);
    rect.setPosition(menuObject.WINDOW_POSITION);
    rect.setFillColor(sf::Color(
        255, 255, 255, transparent_lvl > 255 ? 255 : transparent_lvl++));
    window.draw(rect);
}

}  // namespace

Menu::main_menu::main_menu() {
    prev_win_pos = {0, 0};
    prev_win_size = sf::Vector2f((float)sf::VideoMode::getFullscreenModes().begin()->width,
                                 (float)sf::VideoMode::getFullscreenModes().begin()->height);
    coef = 0.05f;
    ctrl_pressed = false;
    static std::vector<sf::Texture> textures(4);
    textures[0].loadFromFile(R"(data\img\5.png)");
    textures[1].loadFromFile(R"(data\img\6.png)");
    textures[2].loadFromFile(R"(data\img\7.png)");
    textures[3].loadFromFile(R"(data\img\1.png)");

    buttons.emplace_back(0, 0, 100, Menu::EXIT, textures[1]);
    buttons.emplace_back(400, 400, 200, Menu::CHOOSE_THE_MAP, textures[0]);
    buttons.emplace_back(700, 50, 100, Menu::OPEN_LIST_OF_MODS,
                         textures[2]);
    buttons.emplace_back(900, 400, 200, Menu::CREATE_NEW_MAP,
                         textures[3]);
}

bool Menu::main_menu::check_event(sf::RenderWindow &window, sf::Event event, BL::Game_session &gameSession) {
    if (menu::check_event(window, event, gameSession)) {
        return true;
    }
    switch (event.type) {
        case sf::Event::KeyPressed: {
            if (event.key.code == sf::Keyboard::Escape) {
                menuObject.action = Action::CLOSE_THE_WINDOW;
                return true;
            }
        }
        default: {
        } break;
    }
    return false;
}

void Menu::main_menu::run(sf::RenderWindow &window, BL::Game_session &gameSession) {
    sf::Event event{};
    check_scale();
    while (window.isOpen()) {
        if (window.pollEvent(event)) {
            if (check_event(window, event, gameSession)) {
                return;
            }
        }
        menu::run(window, gameSession);
    }
}

Menu::stop_menu::stop_menu(BL::Game_status gameStatus) {
    prev_win_pos = {0, 0};
    prev_win_size = sf::Vector2f((float)sf::VideoMode::getFullscreenModes().begin()->width,
                                 (float)sf::VideoMode::getFullscreenModes().begin()->height);
    coef = 0.05f;
    ctrl_pressed = false;
    static std::vector<sf::Texture> textures(3);
    textures[0].loadFromFile(R"(data\img\1.png)");
    textures[1].loadFromFile(R"(data\img\2.png)");
    textures[2].loadFromFile(R"(data\img\3.png)");

    buttons.emplace_back(400, 400, 150, Menu::RETRY, textures[0]);
    buttons.emplace_back(0, 0, 200, Menu::BACK_TO_MENU, textures[1]);
    if (gameStatus == BL::Game_status::PAUSE) {
        buttons.emplace_back(900, 400, 130, Menu::CONTINUE, textures[2]);
    }
}

bool Menu::stop_menu::check_event(sf::RenderWindow &window, sf::Event event, BL::Game_session &gameSession) {
    if (menu::check_event(window, event, gameSession)) {
        return true;
    }
    switch (event.type) {
        case sf::Event::KeyPressed: {
            if (event.key.code == sf::Keyboard::Escape) {
                menuObject.action = Action::OPEN_MAIN_MENU;
                return true;
            }
        } break;
        default: {
        } break;
    }
    return false;
}

void Menu::stop_menu::run(sf::RenderWindow &window, BL::Game_session &gameSession) {
    sf::Event event{};
    check_scale();
    while (window.isOpen() &&
           (gameSession.get_game_status() == BL::Game_status::PAUSE ||
            gameSession.get_game_status() == BL::Game_status::DEFEAT ||
            gameSession.get_game_status() == BL::Game_status::VICTORY)) {
        if (window.pollEvent(event)) {
            if (check_event(window, event, gameSession)) {
                return;
            }
        }
        menu::run(window, gameSession);
    }
}

bool check_pressing(sf::Vector2f mouse, sf::Vector2f pos, sf::Vector2f sz) {
    if (pos.x <= mouse.x && pos.x + sz.x >= mouse.x &&
        pos.y + sz.y >= mouse.y && pos.y <= mouse.y) {
        return true;
    }
    return false;
}

Menu::scrolling_menu::scrolling_menu(std::string filename_) : filename(std::move(filename_)) {
    transparent_lvl = 0;

    BLOCK_SIZE = {menuObject.WINDOW_SIZE.x / 4,menuObject.WINDOW_SIZE.y / 6};
    track_speed = BLOCK_SIZE.x / 100.f;
    scrolling_speed = BLOCK_SIZE.y / 6.f;
    gap = BLOCK_SIZE.y / 4.f;
    char_size = int(BLOCK_SIZE.y / 4.f);
    font.loadFromFile(R"(data\fonts\GistLight.otf)");

    std::ifstream file(filename);
    std::string map_name;
    while (std::getline(file, map_name)) {
        list_of_maps.emplace_back(map_name);
    }

    int new_size = number_of_blocks();
    blocks_of_map_names.resize(new_size, sf::RectangleShape({BLOCK_SIZE}));
    map_names.resize(new_size);

    first_block = &blocks_of_map_names.front();
    last_block = &blocks_of_map_names.back();
    first_map_name = &map_names.front();
    last_map_name = &map_names.back();
}

bool Menu::scrolling_menu::push(sf::RenderWindow &window, sf::Vector2f mouse) {
    for (int i = 0; i < blocks_of_map_names.size(); i++) {
        if (check_pressing(mouse, blocks_of_map_names[i].getPosition(),
                           blocks_of_map_names[i].getSize())) {
            menuObject.game.map_name = map_names[i].getString();
            return true;
        }
    }
    return false;
}

void Menu::scrolling_menu::blocks_movement() {
    for (int i = 0; i < blocks_of_map_names.size(); i++) {
        sf::RectangleShape &rect = blocks_of_map_names[i];

        std::function<void(float)> move = [&](float var) {
          rect.setSize(rect.getSize() + sf::Vector2f(var, 0));
          map_names[i].setPosition(map_names[i].getPosition() +
                                   sf::Vector2f(var, 0));
          for (int j = 0; j < blocks_of_map_names.size(); j++) {
              sf::RectangleShape &block = blocks_of_map_names[j];
              std::function<sf::Vector2f(sf::Vector2f, sf::Vector2f)> op;
              if (block.getPosition().y < rect.getPosition().y) {
                  op = minus;
              } else if (block.getPosition().y > rect.getPosition().y) {
                  op = plus;
              } else {
                  continue;
              }
              block.setPosition(op(block.getPosition(),
                                sf::Vector2f(0, var / 6)));
              map_names[j].setPosition(op(map_names[j].getPosition(),
                                       sf::Vector2f(0, var / 6)));
          }
        };
        sf::Vector2f pos = (sf::Vector2f)sf::Mouse::getPosition();
        if (check_pressing(pos, menuObject.WINDOW_POSITION, menuObject.WINDOW_SIZE)
            && check_pressing(pos, rect.getPosition(), rect.getSize())) {
            if (rect.getSize().x <= BLOCK_SIZE.x * 1.2) {
                move(track_speed);
            }
        } else if (rect.getSize().x >= BLOCK_SIZE.x) {
                move(-track_speed);
        }
    }
}

int Menu::scrolling_menu::number_of_blocks() const {
    int extra_blocks = 2;
    return std::min((int)(menuObject.WINDOW_SIZE.y / (BLOCK_SIZE.y + gap)) + extra_blocks, (int)list_of_maps.size());
}

void Menu::scrolling_menu::scrolling(const std::function<sf::Vector2f(sf::Vector2f, sf::Vector2f)>& op) {
    for (int i = 0; i < blocks_of_map_names.size(); i++) {
        sf::RectangleShape &rect = blocks_of_map_names[i];
        rect.setPosition(
            op(rect.getPosition(),
               sf::Vector2f(0, scrolling_speed)));
        map_names[i].setPosition(
            op(map_names[i].getPosition(),
               sf::Vector2f(0, scrolling_speed)));

    }
}

sf::Vector2f Menu::scrolling_menu::plus(sf::Vector2f a, sf::Vector2f b) {
    return a + b;
}

sf::Vector2f Menu::scrolling_menu::minus(sf::Vector2f a, sf::Vector2f b) {
    return a - b;
}
void Menu::scrolling_menu::hide_protruding_blocks(sf::RenderWindow &window) {
    sf::RectangleShape block1(sf::Vector2f(BLOCK_SIZE.x + BLOCK_SIZE.x / 4,
                                           menuObject.WINDOW_POSITION.y + (float)window.getPosition().y));
    sf::RectangleShape block2(block1.getSize());
    block1.setPosition(menuObject.WINDOW_POSITION.x, (float)window.getPosition().y);
    block2.setPosition(menuObject.WINDOW_POSITION + sf::Vector2f(0, menuObject.WINDOW_SIZE.y));
    block1.setFillColor(sf::Color::Black);
    block2.setFillColor(sf::Color::Black);
    window.draw(block1);
    window.draw(block2);
}

void Menu::scrolling_menu::run(sf::RenderWindow &window) {
    if (!blocks_of_map_names.empty()) {
        rebuild();
        sf::Event event{};
        while (window.isOpen()) {
            if (window.pollEvent(event)) {
                if (check_event(window, event)) {
                    return;
                }
            }
            draw(window);
        }
    } else {
        menuObject.action = Action::OPEN_MAIN_MENU;
        return;
    }
}

bool Menu::scrolling_menu::check_event(sf::RenderWindow &window, sf::Event event) {
    std::size_t size = blocks_of_map_names.size();
    switch (event.type) {
        case sf::Event::MouseButtonPressed: {
            if (event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f pos = (sf::Vector2f)sf::Mouse::getPosition();
                if (check_pressing(pos, menuObject.WINDOW_POSITION,menuObject.WINDOW_SIZE)) {
                    if (push(window, pos)) {
                        menuObject.action = Action::OPEN_GAME_MAP;
                        return true;
                    }
                }
            }
        } break;
        case sf::Event::KeyPressed: {
            if (event.key.code == sf::Keyboard::Escape) {
                menuObject.action = Action::OPEN_MAIN_MENU;
                return true;
            }
        } break;
        case sf::Event::MouseWheelScrolled: {
            if (event.mouseWheelScroll.delta < 0) {
                if (delta + blocks_of_map_names.size() >= list_of_maps.size()) {
                    scrolling(minus);
                    break;
                }
                if (menuObject.WINDOW_POSITION.y + menuObject.WINDOW_SIZE.y >= last_block->getPosition().y +
                                                                               BLOCK_SIZE.y -
                                                                               scrolling_speed) {
                    first_block->setPosition(sf::Vector2f(
                        menuObject.WINDOW_POSITION.x + 5,
                        last_block->getPosition().y + BLOCK_SIZE.y + gap));
                    last_block = first_block;                                    //  нижний блок
                    first_block =
                        &blocks_of_map_names[(delta + 1 + size) % size];       //  верхний блок

                    first_map_name->setPosition(sf::Vector2f(
                        menuObject.WINDOW_POSITION.x + 5,
                        last_map_name->getPosition().y + BLOCK_SIZE.y + gap));
                    first_map_name->setString(list_of_maps[size + delta]);

                    last_map_name = first_map_name;                              //  нижний блок
                    first_map_name =
                        &map_names[(delta + 1 + size) % size];   //  верхний блок
                    delta++;
                }
                scrolling(minus);
            } else if (event.mouseWheelScroll.delta > 0) {
                if (delta == 0) {
                    scrolling(plus);
                    break;
                }
                if (menuObject.WINDOW_POSITION.y <=
                    first_block->getPosition().y +
                    scrolling_speed) {
                    last_block->setPosition(sf::Vector2f(
                        menuObject.WINDOW_POSITION.x + 5,
                        first_block->getPosition().y - BLOCK_SIZE.y - gap));
                    first_block = last_block;                                      //  верхний блок
                    last_block =
                        &blocks_of_map_names[(--delta - 1 + size) % size]; //  нижний блок

                    last_map_name->setPosition(sf::Vector2f(
                        menuObject.WINDOW_POSITION.x + 5,
                        first_map_name->getPosition().y - BLOCK_SIZE.y - gap));
                    last_map_name->setString(list_of_maps[delta]);
                    first_map_name = last_map_name;                           //  нижний блок
                    last_map_name =
                        &map_names[(delta - 1 + size) % size];                 //  верхний блок

                }
                scrolling(plus);
            }
        } break;
        default: {
            menuObject.action = Action::NOTHING;
            return false;
        } break;
    }
    return false;
}

void Menu::scrolling_menu::draw(sf::RenderWindow &window) {
    background_draw(window);
    menuObject.mainMenu.draw(window);
    blocks_movement();
    for (int i = 0; i < blocks_of_map_names.size(); i++) {
        window.draw(blocks_of_map_names[i]);
        window.draw(map_names[i]);
    }
    hide_protruding_blocks(window);
    window.display();
}

void Menu::scrolling_menu::rebuild() {
    delta = 0;
    BLOCK_SIZE = {menuObject.WINDOW_SIZE.x / 4, menuObject.WINDOW_SIZE.y / 6};
    track_speed = BLOCK_SIZE.x / 100.f;
    scrolling_speed = BLOCK_SIZE.y / 6.f;
    gap = BLOCK_SIZE.y / 4.f;
    char_size = int(BLOCK_SIZE.y / 4.f);

    list_of_maps.clear();
    std::ifstream file(filename);
    std::string map_name;
    while (std::getline(file, map_name)) {
        list_of_maps.emplace_back(map_name);
    }

    float start_y_pos = menuObject.WINDOW_POSITION.y;
    int new_size = number_of_blocks();

    blocks_of_map_names.assign(new_size, sf::RectangleShape(BLOCK_SIZE));

    map_names.resize(new_size);

    for (int i = 0; i < blocks_of_map_names.size(); i++) {
        blocks_of_map_names[i].setFillColor(sf::Color(255, 114, 219, 200));
        blocks_of_map_names[i].setOutlineThickness(5);
        blocks_of_map_names[i].setOutlineColor(
            sf::Color(248, 137, 90, 159));
        map_names[i].setStyle(sf::Text::Bold);
        map_names[i].setFont(font);
        blocks_of_map_names[i].setPosition(menuObject.WINDOW_POSITION.x + 5,
                                           start_y_pos + (BLOCK_SIZE.y + gap) * (float)i);
        map_names[i].setString(list_of_maps[i]);
        map_names[i].setPosition(blocks_of_map_names[i].getPosition() + sf::Vector2f(5, 5));
        map_names[i].setFillColor(text_color);
        map_names[i].setCharacterSize(char_size);
    }
    first_block = &blocks_of_map_names.front();
    last_block = &blocks_of_map_names.back();
    first_map_name = &map_names.front();
    last_map_name = &map_names.back();
}

Menu::map_creation_menu::map_creation_menu(std::string filename_) : filename(std::move(filename_)) {
    BLOCK_SIZE = {menuObject.WINDOW_SIZE.x / 4.f, menuObject.WINDOW_SIZE.y / 6.f};
    gap = BLOCK_SIZE.y / 4.f;
    char_size = int(BLOCK_SIZE.y / 4.f);

    font.loadFromFile(R"(data\fonts\GistLight.otf)");

    list_of_data.resize(5);
    blocks_of_map_data.resize(5, sf::RectangleShape{BLOCK_SIZE});
    text.resize(5);
    for (int i = 0; i < 5; i++) {
        blocks_of_map_data[i].setOutlineThickness(5);
        blocks_of_map_data[i].setOutlineColor(sf::Color(35, 184, 84, 103));
        blocks_of_map_data[i].setFillColor(sf::Color(41, 177, 177, 103));
        text[i].setFont(font);
        text[i].setStyle(sf::Text::Bold);
    }
    create_block = random_map_block = blocks_of_map_data[0];

    std::string address;
    std::vector<std::string> addresses_of_images, addresses_of_music;
    std::ifstream file1("data/images.txt"), file2("data/music.txt");
    while (std::getline(file1, address)) {
        addresses_of_images.emplace_back(address);
    }
    while (std::getline(file2, address)) {
        addresses_of_music.emplace_back(address);
    }
    list_of_default_data.resize(10);
    for (int  i = 0; i < 10; i++) {
        list_of_default_data[i].resize(5);
        list_of_default_data[i][0] = rand() % 2 == 0 ? "Aim" : "Conveyor";
        list_of_default_data[i][1] = "Anonymous";
        list_of_default_data[i][2] = "New_map";
        list_of_default_data[i][3] = addresses_of_music[rand() % addresses_of_music.size()];
        list_of_default_data[i][4] = addresses_of_images[rand() % addresses_of_images.size()];
    }
}

void Menu::map_creation_menu::rebuild() {
    BLOCK_SIZE = {menuObject.WINDOW_SIZE.x / 4.f, menuObject.WINDOW_SIZE.y / 8.f};
    gap = BLOCK_SIZE.y / 4.f;
    char_size = int(BLOCK_SIZE.y / 4.f);

    float pos_y = menuObject.WINDOW_POSITION.y + gap;

    for (int i = 0; i < 5; i++) {
        blocks_of_map_data[i].setPosition(menuObject.WINDOW_POSITION.x + 5,
                                          pos_y + (BLOCK_SIZE.y + gap) * (float)i);
        blocks_of_map_data[i].setSize(BLOCK_SIZE);
        text[i].setCharacterSize(char_size);
        text[i].setFillColor(text_color);
        text[i].setPosition(blocks_of_map_data[i].getPosition() + sf::Vector2f(5, 5));
    }
    named_block = text[0];
    create_block = random_map_block = blocks_of_map_data[0];
    random_map_block.setPosition(blocks_of_map_data[0].getPosition() + sf::Vector2f(BLOCK_SIZE.x + gap, 0));
    create_block.setPosition(blocks_of_map_data[4].getPosition() + sf::Vector2f(0, BLOCK_SIZE.y + gap));
    random_map_block.setSize({BLOCK_SIZE.y * 2, BLOCK_SIZE.y * 2});
}

void Menu::map_creation_menu::fix_map_name(std::string &cur_map_name) const {
    std::unordered_map<std::string, int> names;
    std::ifstream file(filename);
    std::string map_name;
    while (std::getline(file, map_name)) {
        names[map_name]++;
    }
    int id = 1;
    std::size_t name_size = cur_map_name.length();
    while (names.find(cur_map_name) != names.end()) {
        cur_map_name =
            cur_map_name.substr(0, name_size)
            + '(' + std::to_string(id) + ')';
        id++;
    }
}

void Menu::map_creation_menu::run(sf::RenderWindow &window) {
    rebuild();
    sf::Event event{};
    while (window.isOpen()) {
        if (window.pollEvent(event)) {
            if (check_event(window, event)) {
                return;
            }
        }
        draw(window);
        window.display();
    }
}

void Menu::map_creation_menu::draw(sf::RenderWindow &window) {
    window.clear();
    background_draw(window);
    for (int i = 0; i < 5; i++) {
        window.draw(blocks_of_map_data[i]);
        window.draw(text[i]);
    }

    window.draw(create_block);
    window.draw(random_map_block);
    named_block.setPosition(create_block.getPosition() + sf::Vector2f(5, 5));
    named_block.setString("Create!");
    window.draw(named_block);
    named_block.setString("Random");
    named_block.setPosition(random_map_block.getPosition() + sf::Vector2f(5, 5));
    window.draw(named_block);
}

bool Menu::map_creation_menu::check_event(sf::RenderWindow &window, sf::Event event) {
    static int index = -1;
    switch (event.type) {
        case sf::Event::MouseButtonPressed: {
            if (event.mouseButton.button == sf::Mouse::Left) {
                index = get_id(sf::Vector2f(sf::Mouse::getPosition()));
            }
            if (create_or_generate((sf::Vector2f)sf::Mouse::
                                       getPosition()) == CREATE) {
                for (int i = 0 ; i < 5; i++) {
                    if (list_of_data[i].empty()) {
                        list_of_data[i] = list_of_default_data[0][i];
                        text[i].setString(list_of_data[i]);
                    }
                }
                if (selection_menu(window)) {
                    fix_map_name(list_of_data[2]);
                    new_map_name = list_of_data[2];
                    if (list_of_data[0] == "Aim") {
                        USO::Aim_map test(
                            list_of_data[0],
                            list_of_data[2],
                            "data/maps/" +
                            list_of_data[2] +
                            ".txt",
                            R"(data\music\)" +
                            list_of_data[3] +
                            ".ogg",
                            list_of_data[3],
                            R"(data\img\)" +
                            list_of_data[4] +
                            ".png",
                            "data/fonts/aller.ttf",
                            "data/sounds/click.ogg");
                        test.constructor_run(
                            window);
                    } else if (list_of_data[0] == "Conveyor") {
                        USO::Conveyor_map test(
                            "Conveyor",
                            list_of_data[2],
                            "data/maps/" +
                            list_of_data[2] +
                            ".txt",
                            R"(data\music\)" +
                            list_of_data[3] +
                            ".ogg",
                            list_of_data[3],
                            R"(data\img\)" +
                            list_of_data[4] +
                            ".png",
                            "data/fonts/aller.ttf",
                            "data/sounds/click.ogg");
                        test.constructor_run(window);
                    }
                    return true;
                }
                return true;
            } else if (create_or_generate((sf::Vector2f)sf::Mouse::getPosition()) == RANDOM_GENERATE) {
                list_of_data = list_of_default_data[rand() % 10];
                for (int i = 0; i < 5; i++) {
                    text[i].setString(list_of_data[i]);
                }
            }
        } break;
        case sf::Event::KeyPressed: {
            if (event.key.code == sf::Keyboard::Escape) {
                list_of_data.assign(5, {});
                menuObject.action = Action::OPEN_MAIN_MENU;
                return true;
            }
            if (event.key.code == sf::Keyboard::Enter) {
                index = -1;
                break;
            }
            if (event.key.code == sf::Keyboard::BackSpace) {
                if (index != -1 && !list_of_data[index].empty()) {
                    list_of_data[index].pop_back();
                    text[index].setString(list_of_data[index]);
                }
            }
        } break;
        case sf::Event::TextEntered: {
            if (index != -1 && float(list_of_data[index].size() * char_size) <= BLOCK_SIZE.x) {
                if (event.text.unicode > 33 && event.text.unicode < 127) {
                    list_of_data[index].push_back((char)event.text.unicode);
                    text[index].setString(list_of_data[index]);
                }
            }
        } break;
        default: {
        } break;
    }
    menuObject.action = Action::NOTHING;
    return false;
}

bool Menu::map_creation_menu::selection_menu(sf::RenderWindow &window) {
    static sf::Texture img;
    img.loadFromFile(R"(data\img\grad.png)");
    sf::Vector2f center((float)window.getSize().x / 2, (float)window.getSize().y / 2);
    float radius = 200.f;
    Button back_to_main(center.x - 1.5f * radius,
                               center.y, radius, Event::BACK_TO_MENU, img);
    Button open_constr(center.x + 1.5f * radius,
                               center.y, radius, Event::OPEN_CONSTR, img);

    float coef = menuObject.WINDOW_SIZE.x / (float)window.getSize().x - 1.f;
    back_to_main.changePosition(coef, menuObject.WINDOW_POSITION);
    open_constr.changePosition(coef, menuObject.WINDOW_POSITION);
    back_to_main.changeSize(coef, menuObject.WINDOW_SIZE.x);
    open_constr.changeSize(coef, menuObject.WINDOW_SIZE.x);

    sf::Event event{};
    BL::Game_session gameSession;
    while (true) {
        window.clear();
        if (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::MouseButtonPressed: {
                    if (open_constr.press(window, sf::Vector2f(sf::Mouse::getPosition()), gameSession)) {
                        return true;
                    }
                    if (back_to_main.press(window, sf::Vector2f(sf::Mouse::getPosition()), gameSession)) {
                        return false;
                    }
                } break;
                default: {
                } break;
            }
        }
        draw(window);
        open_constr.draw(window);
        back_to_main.draw(window);
        window.display();
    }
}

int Menu::map_creation_menu::get_id(sf::Vector2f mouse) const {
    for (int i = 0; i < 5; i++) {
        if (check_pressing(mouse, blocks_of_map_data[i].getPosition(),
                           blocks_of_map_data[i].getSize())) {
            return i;
        }
    }
    return -1;
}

Menu::CG Menu::map_creation_menu::create_or_generate(sf::Vector2f mouse) {
    if (check_pressing(mouse, create_block.getPosition(), create_block.getSize())) {
        return CREATE;
    } else if (check_pressing(mouse, random_map_block.getPosition(),
                              random_map_block.getSize())) {
        return RANDOM_GENERATE;
    }
    return SCREEN;
}

Menu::mod_menu::mod_menu() {
    mod_blocks.resize(NUMBER_OF_MODS);

    std::ifstream file(R"(C:\Users\bon52\project\USO\data\mods)");
    std::string mod_name;

    font.loadFromFile(R"(data\fonts\aller.ttf)");

    while (std::getline(file, mod_name)) {
        list_of_mods.emplace_back(sf::Text(mod_name, font, 40));
    }

    for (int i = 0; i < NUMBER_OF_MODS; i++) {
        mod_blocks[i].setPosition(sf::Vector2f(900, float(70 * (i + 1))));
        mod_blocks[i].setSize(sf::Vector2f(400, 70));
        mod_blocks[i].setFillColor(sf::Color(255, 255, 255));
        mod_blocks[i].setOutlineColor(sf::Color(0, 0, 0));
        list_of_mods[i].setPosition(sf::Vector2f(905, (float)(i + 1) * 70));
        list_of_mods[i].setFillColor(sf::Color(0, 0, 0));
    }
}

float Menu::mod_menu::return_acceleration(const sf::Text& text) {
    std::string str = text.getString();
    if (str[0] == '.') {
        str = '0' + str;
    }
    float num = std::stof(str);
    if (num > 2) {
        num = 2;
    }
    if (num < 0.5) {
        num = 0.5;
    }
    return num;
}

void Menu::mod_menu::draw(sf::RenderWindow &window) {
    int index = -1;

    while (window.isOpen()) {
        window.clear();
        sf::Event event{};
        if (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::KeyReleased: {
                    if (event.key.code == sf::Keyboard::Escape) {
                        return;
                    }
                    if (event.key.code == sf::Keyboard::BackSpace) {
                        if (index != -1) {
                            std::size_t size =
                                list_of_mods[index].getString().getSize();
                            if (size == 0) {
                                list_of_mods[index].setString(
                                    "enter the number");
                                list_of_mods[index].setFillColor(input_color);
                            } else {
                                list_of_mods[index].setString(
                                    list_of_mods[index].getString().substring(
                                        0, size - 1));
                            }
                        }
                    }
                    if (event.key.code == sf::Keyboard::Enter) {
                        if (index == Menu::ACCELERATION) {
                            acceleration_factor =
                                return_acceleration(list_of_mods[index]);
                        }
                        index = -1;
                    }
                } break;
                case sf::Event::MouseButtonReleased: {
                    index = get_id(mod_blocks, (sf::Vector2f)sf::Mouse::getPosition());
                    if (index == Menu::ACCELERATION) {
                        list_of_mods[index].setString("enter the number");
                        list_of_mods[index].setFillColor(input_color);
                    }
                } break;
                case sf::Event::MouseButtonPressed: {
                    index = get_id(mod_blocks, (sf::Vector2f)sf::Mouse::getPosition());
                    if (index == -1) {
                        return;
                    }
                } break;
                case sf::Event::TextEntered: {
                    if (index != -1) {
                        if ((event.text.unicode >= 48 && event.text.unicode <= 57)
                            || event.text.unicode == 46) {
                            if (check_color(list_of_mods[index])) {
                                list_of_mods[index].setString("");
                                list_of_mods[index].setFillColor(text_color);
                            }
                            list_of_mods[index].setString(
                                list_of_mods[index].getString() +
                                (char)event.text.unicode);
                        }
                    }
                } break;
                default: {
                } break;
            }
        }
        for (int i = 0; i < NUMBER_OF_MODS; i++) {
            window.draw(mod_blocks[i]);
            window.draw(list_of_mods[i]);
        }
        window.display();
    }
}

bool Menu::mod_menu::check_color(const sf::Text& text) {
    return text.getFillColor() == input_color;
}

int Menu::mod_menu::get_id(std::vector<sf::RectangleShape> &blocks, sf::Vector2f mouse) {
    for (int i = 0; i < NUMBER_OF_MODS; i++) {
        if (check_pressing(mouse, blocks[i].getPosition(),
                           blocks[i].getSize())) {
            return i;
        }
    }
    return -1;
}

void Menu::change_speed(const std::string &file_name, float coef) {
    std::vector<std::string> list_of_data;
    std::ifstream read_file(file_name);
    std::string data;
    while (std::getline(read_file, data)) {
        list_of_data.emplace_back(data);
        if (data == "Aim_circle" || data == "Aim_slider" || data == "Aim_spinner") {
            float start, duration;
            read_file >> start >> duration;
            start *= coef;
            duration *= coef;
            list_of_data.emplace_back(std::to_string((int)start) + "\n" +
                                      std::to_string((int)duration));
        }
    }
    std::ofstream overwrite_file(file_name, std::ios::out | std::ios::trunc);
    for (auto &i : list_of_data) {
        if (!i.empty()) {
            overwrite_file << i << "\n";
        }
    }
}

void Menu::menu::draw(sf::RenderWindow &window) {
    for (auto &button : buttons) {
        button.draw(window);
    }
    static sf::CircleShape mouse(5.f);
    mouse.setPosition((sf::Vector2f)sf::Mouse::getPosition());
    mouse.setFillColor(color_for_mouse);
    window.draw(mouse);
}

bool Menu::menu::check_event(sf::RenderWindow &window, sf::Event event,
                             BL::Game_session &gameSession) {
    switch (event.type) {
        case sf::Event::KeyPressed: {
            if (event.key.code == sf::Keyboard::LControl
                || event.key.code == sf::Keyboard::RControl) {
                ctrl_pressed = true;
            }
        } break;
        case sf::Event::KeyReleased: {
            if (event.key.code == sf::Keyboard::LControl
                || event.key.code == sf::Keyboard::RControl) {
                ctrl_pressed = false;
            }
        } break;
        case sf::Event::MouseButtonPressed: {
            if (event.mouseButton.button == sf::Mouse::Left) {
                for (auto &button : buttons) {
                    if (button.press(window,
                                 sf::Vector2f(sf::Mouse::getPosition()),
                                 gameSession)) {
                        return true;
                    }
                }
            }
        } break;
        case sf::Event::MouseWheelScrolled: {
            if (ctrl_pressed) {
                static sf::Vector2f center = {float(window.getSize().x) / 2,
                                              float(window.getSize().y) / 2};
                float const_coef = coef;
                if (event.mouseWheelScroll.delta < 0) {
                    if (menuObject.WINDOW_SIZE.x < float(window.getSize().x) / 2.f) {
                        break;
                    }
                    coef *= -1;
                } else if (event.mouseWheelScroll.delta > 0) {
                    float delta = (float)window.getSize().x - menuObject.WINDOW_SIZE.x;
                    float eps = 1;
                    if (delta <= coef * (float)window.getSize().x) {
                        coef = 1.f - menuObject.WINDOW_SIZE.x / (float)window.getSize().x;
                    }
                    if (std::abs(delta) <= eps) {
                        coef = const_coef;
                        break;
                    }
                }
                prev_win_pos = menuObject.WINDOW_POSITION;
                prev_win_size = menuObject.WINDOW_SIZE;
                menuObject.WINDOW_SIZE += sf::Vector2f(menuObject.WINDOW_SIZE.x * coef, menuObject.WINDOW_SIZE.y * coef);
                menuObject.WINDOW_POSITION = sf::Vector2f(center.x - menuObject.WINDOW_SIZE.x / 2,
                                                          center.y - menuObject.WINDOW_SIZE.y / 2);
                for (auto &button : buttons) {
                    button.changeSize(coef, menuObject.WINDOW_SIZE.x);
                    button.changePosition(coef, menuObject.WINDOW_POSITION, prev_win_pos);
                }
                coef = const_coef;
            }
        } break;
        default: {
            menuObject.action = Action::NOTHING;
            return false;
        } break;
    }
    return false;
}

void Menu::menu::run(sf::RenderWindow &window, BL::Game_session &gameSession) {
    window.clear();
    for (auto &button : buttons) {
        button.guidance(sf::Vector2f(sf::Mouse::getPosition()));
    }
    background_draw(window);
    draw(window);
    window.display();
}

void Menu::menu::check_scale() {
    if (prev_win_size != menuObject.WINDOW_SIZE) {
        float tmp_coef = menuObject.WINDOW_SIZE.x / prev_win_size.x - 1.f;
        for (auto &button : buttons) {
            button.changeSize(tmp_coef, menuObject.WINDOW_SIZE.x);
            button.changePosition(tmp_coef, menuObject.WINDOW_POSITION, prev_win_pos);
        }
        prev_win_size = menuObject.WINDOW_SIZE;
        prev_win_pos = menuObject.WINDOW_POSITION;
    }
}