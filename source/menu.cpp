#include "menu.h"
#include <fstream>
#include <utility>
#include <vector>
#include "maps.h"
#include "menu_objects.h"
#include <string>
#include <unordered_map>
#include <random>

namespace {

const sf::Color color_for_mouse = sf::Color(241, 200, 14);
const sf::Color input_color = sf::Color(150, 160, 145);
const sf::Color text_color = sf::Color(2, 2, 2);
float acceleration_factor = 1;

int transparent_lvl = 0;

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
void draw_menu(sf::RenderWindow &window) {
    static sf::Texture img;
    img.loadFromFile(R"(data\img\back.png)");
    sf::RectangleShape rect(static_cast<sf::Vector2f>(window.getSize()));
    rect.setTexture(&img);
    rect.setPosition(0, 0);
    rect.setFillColor(sf::Color(
        255, 255, 255, transparent_lvl > 255 ? 255 : transparent_lvl++));
    window.draw(rect);
}

}  // namespace

static std::random_device r;
static std::default_random_engine e1(r());

sf::RenderWindow &Menu::set_settings() {
    sf::ContextSettings setting;
    setting.antialiasingLevel = 8;
    static sf::RenderWindow window(sf::VideoMode(1080, 720), "USO!",
                                   sf::Style::Fullscreen, setting);
    window.setMouseCursorVisible(true);
    window.display();
    return window;
}

void Menu::menu(sf::RenderWindow &window, BL::Game_session gameSession) {
    transparent_lvl = 0;
    std::vector<Menu::Button> buttons;
    std::vector<sf::Texture> textures(4);
    textures[0].loadFromFile(R"(data\img\5.png)");
    textures[1].loadFromFile(R"(data\img\6.png)");
    textures[2].loadFromFile(R"(data\img\7.png)");
    textures[3].loadFromFile(R"(data\img\1.png)");

    buttons.emplace_back(400, 400, 200, Menu::CHOOSE_THE_MAP, textures[0]);
    buttons.emplace_back(0, 0, 100, Menu::EXIT, textures[1]);
    buttons.emplace_back(700, 50, 100, Menu::OPEN_LIST_OF_MODS,
                         textures[2]);
    buttons.emplace_back(900, 400, 200, Menu::CREATE_NEW_MAP,
                         textures[3]);


    sf::CircleShape mouse(5.f);

    while (window.isOpen()) {
        window.clear();
        draw_menu(window);
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
                                 static_cast<float>(event.mouseButton.y)},
                                gameSession);
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
            mouse.setFillColor(color_for_mouse);
            window.draw(mouse);
        }
        window.display();
    }
}

void Menu::stop_menu(sf::RenderWindow &window, BL::Game_session &gameSession) {
    transparent_lvl = 0;
    std::vector<Menu::Button> buttons;
    std::vector<sf::Texture> textures(3);
    textures[0].loadFromFile(R"(data\img\1.png)");
    textures[1].loadFromFile(R"(data\img\2.png)");
    textures[2].loadFromFile(R"(data\img\3.png)");
    buttons.emplace_back(400, 400, 150, Menu::RETRY, textures[0]);
    buttons.emplace_back(0, 0, 200, Menu::BACK_TO_MENU, textures[1]);
    if (gameSession.get_game_status() == BL::Game_status::PAUSE) {
        buttons.emplace_back(900, 400, 130, Menu::CONTINUE, textures[2]);
    }

    sf::CircleShape mouse(5.f);
    while (window.isOpen() &&
           (gameSession.get_game_status() == BL::Game_status::PAUSE ||
            gameSession.get_game_status() == BL::Game_status::DEFEAT ||
            gameSession.get_game_status() == BL::Game_status::VICTORY)) {

        sf::Event event{};
        draw_menu(window);
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
                                         (sf::Vector2f)sf::Mouse::getPosition(),
                                         gameSession);
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
            mouse.setFillColor(color_for_mouse);
            window.draw(mouse);
        }
        window.display();
    }
}

void Menu::constructor_menu(sf::Window &window) {
    std::vector<std::string> saved_maps(get_vector_of_saved_maps_names());
    sf::Vector2f sizes = get_constructor_menu_buttons_size();

    std::vector<Menu::Button> buttons;

    for (auto &map_name : saved_maps) {
        //        buttons.emplace_back(start_x, start_y, );
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
    std::ifstream file(filename);
    std::string map_name;

    while (std::getline(file, map_name)) {
        list_of_maps.emplace_back(map_name);
    }

    for (int i = 0; i < std::min(MAX_SIZE, list_of_maps.size()); i++) {
        blocks_of_maps_name.emplace_back(sf::Vector2f(400, 70));
        blocks_of_maps_name[i].setPosition(5, (float)(i + 5) * 70);
        blocks_of_maps_name[i].setOutlineThickness(5);
        blocks_of_maps_name[i].setOutlineColor(sf::Color(0, 0, 0));
    }
    font.loadFromFile(R"(data\fonts\GistLight.otf)");
    text.setFont(font);
    text.setCharacterSize(40);
    text.setFillColor(sf::Color(34, 46, 137));
    text.setStyle(sf::Text::Bold);

    text.setFillColor(sf::Color(2, 2, 2));
    text.setScale(0.7, 0.7);
}

Menu::map_creation_menu::map_creation_menu(std::string filename_) : filename(std::move(filename_)) {
    for (int i = 0; i < 5; i++) {
        blocks_of_map_data.emplace_back(sf::Vector2f(600, 100));
        blocks_of_map_data[i].setPosition(0, (float)i * 100);
        blocks_of_map_data[i].setOutlineThickness(5);
        blocks_of_map_data[i].setOutlineColor(sf::Color(0, 0, 0));
    }
    font.loadFromFile(R"(data\fonts\GistLight.otf)");
    text.setFont(font);
    text.setCharacterSize(40);
    text.setFillColor(sf::Color(34, 46, 137));
    text.setStyle(sf::Text::Bold);
    text.setFillColor(sf::Color(2, 2, 2));
    text.setScale(0.7, 0.7);

    random_map_block.setSize(sf::Vector2f(600, 100));
    random_map_block.setPosition(600, 0);
    random_map_block.setOutlineThickness(5);
    random_map_block.setOutlineColor(sf::Color(0, 0, 0));

    create_block.setSize(sf::Vector2f(600, 100));
    create_block.setPosition(0, 500);
    create_block.setOutlineThickness(5);
    create_block.setOutlineColor(sf::Color(0, 0, 0));
    list_of_data.resize(5);

    std::string address;
    std::vector<std::string> addresses_of_images, addresses_of_music;
    std::ifstream file1("data/images.txt"), file2("data/music.txt");
    while (std::getline(file1, address)) {
        addresses_of_images.emplace_back(address);
    }
    while (std::getline(file2, address)) {
        addresses_of_music.emplace_back(address);
    }

    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> rand(1, 1e4);

    list_of_default_data.resize(10);
    for (int  i = 0; i < 10; i++) {
        list_of_default_data[i].resize(5);
        list_of_default_data[i][0] = rand(e1) % 2 == 0 ? "Aim" : "Conveyor";
        list_of_default_data[i][1] = "Anonymous";
        list_of_default_data[i][2] = "New_map";
        list_of_default_data[i][3] = addresses_of_music[rand(e1) % addresses_of_music.size()];
        list_of_default_data[i][4] = addresses_of_images[rand(e1) % addresses_of_images.size()];
    }
}

bool Menu::scrolling_menu::push(sf::RenderWindow &window, sf::Vector2f mouse) {
    for (int i = 0; i < blocks_of_maps_name.size(); i++) {
        if (check_pressing(mouse, blocks_of_maps_name[i].getPosition(),
                           blocks_of_maps_name[i].getSize())) {
            std::ifstream file(R"(data\maps\)" + list_of_maps[i + delta] + ".txt");
            std::string mod;
            std::getline(file, mod);
            if (mod == "Conveyor") {
                USO::Conveyor_map test(R"(data\maps\)" +
                                       list_of_maps[i + delta] + ".txt");
                test.run(window);
            } else {

                change_speed(R"(data\maps\)" + list_of_maps[i + delta] +
                             ".txt", 1.f / acceleration_factor);

                USO::Aim_map test(R"(data\maps\)" + list_of_maps[i + delta] +
                                  ".txt");
                test.run(window);

                change_speed(R"(data\maps\)" + list_of_maps[i + delta] +
                             ".txt", acceleration_factor);
            }
            return false;
        }
    }
    return true;
}

int Menu::scrolling_menu::get_delta() const {
    return delta;
}

void Menu::scrolling_menu::scrolling_down() {
    if (delta + std::min(MAX_SIZE, list_of_maps.size()) < list_of_maps.size()) {
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
    std::vector<Menu::Button> buttons;
    std::vector<sf::Texture> textures(4);
    textures[0].loadFromFile(R"(data\img\5.png)");
    textures[1].loadFromFile(R"(data\img\6.png)");
    textures[2].loadFromFile(R"(data\img\7.png)");
    textures[3].loadFromFile(R"(data\img\1.png)");
    buttons.emplace_back(400, 400, 200, Menu::CHOOSE_THE_MAP, textures[0]);
    buttons.emplace_back(0, 0, 100, Menu::EXIT, textures[1]);
    buttons.emplace_back(700, 50, 100, Menu::OPEN_LIST_OF_MODS, textures[2]);
    buttons.emplace_back(900, 400, 200, Menu::CREATE_NEW_MAP, textures[3]);

    while (window.isOpen()) {
        window.clear();
        sf::Event event{};
        if (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::MouseButtonPressed: {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        if (push(window,
                                 (sf::Vector2f)sf::Mouse::getPosition())) {
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
                default: {
                } break;
            }
        }
        draw_menu(window);
        for (auto &button : buttons) {
            button.draw(window);
        }
        for (int i = 0; i < std::min(MAX_SIZE, list_of_maps.size()); i++) {
            window.draw(blocks_of_maps_name[i]);
            text.setString(list_of_maps[i + delta]);
            text.setPosition(5, (float)(i + 5) * 70);
            window.draw(text);
            mouse.setPosition((sf::Vector2f)sf::Mouse::getPosition());
            mouse.setFillColor(color_for_mouse);
            window.draw(mouse);
        }
        window.display();
    }
}

void Menu::map_creation_menu::add_new_map(const std::string &map_name) const {
    std::ofstream file(filename, std::ios::app);
    file << map_name << std::endl;
}

void Menu::map_creation_menu::draw(sf::RenderWindow &window) {
    sf::CircleShape mouse(5.f);
    int index = -1;
    while (window.isOpen()) {
        window.clear();
        sf::Event event{};
        if (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::MouseButtonPressed: {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        index = get_id((sf::Vector2f)sf::Mouse::getPosition());
                        if (create_or_generate(window, (sf::Vector2f)sf::Mouse::getPosition()) == CREATE) {
                            for (int i = 0 ; i < 5; i++) {
                                if (list_of_data[i].empty()) {
                                    list_of_data[i] = list_of_default_data[0][i];
                                }
                            }
                            bool is_escape_pressed = false;

                            sf::CircleShape constr(200.f);
                            sf::CircleShape back_to_main_menu(200.f);

                            constr.setPosition((float)sf::VideoMode::getFullscreenModes().begin()->width / 3 - 200.f,
                                            (float)sf::VideoMode::getFullscreenModes().begin()->height / 2 + 70);
                            back_to_main_menu.setPosition(2.f * (float)sf::VideoMode::getFullscreenModes().begin()->width / 3 - 200.f,
                                           (float)sf::VideoMode::getFullscreenModes().begin()->height / 2 + 70);
                            back_to_main_menu.setFillColor(sf::Color(210, 0, 0));
                            constr.setFillColor(sf::Color(0, 210, 21));

                            while (true) {
                                window.clear();

                                if (window.pollEvent(event)) {
                                    switch (event.type) {
                                        case sf::Event::KeyPressed: {
                                            if (event.key.code == sf::Keyboard::Escape) {
                                                is_escape_pressed = true;
                                            }
                                        } break;
                                        case sf::Event::MouseButtonReleased: {
                                            if (USO::Aim_circle::is_circle_correct_click(
                                                    sf::Vector2f(sf::Mouse::getPosition(window)),
                                                        constr.getPosition() +
                                                        sf::Vector2f(
                                                                constr
                                                                    .getRadius(),
                                                                constr
                                                                    .getRadius()),
                                                        constr.getRadius())) {
                                                std::unordered_map<std::string, int> names;
                                                std::ifstream file(filename);
                                                std::string map_name;
                                                while (std::getline(file, map_name)) {
                                                    names[map_name]++;
                                                }
                                                int id = 1;
                                                std:size_t name_size = list_of_data[2].size();
                                                while (names.find(list_of_data[2]) != names.end()) {
                                                    list_of_data[2] =
                                                        list_of_data[2].substr(0, name_size)
                                                        + "(" + std::to_string(id) + ")";
                                                    id++;
                                                }

                                                add_new_map(list_of_data[2]);

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
                                                return;
                                            } else if (USO::Aim_circle::is_circle_correct_click(
                                                sf::Vector2f(sf::Mouse::getPosition(window)),
                                                back_to_main_menu.getPosition() +
                                                sf::Vector2f(back_to_main_menu.getRadius(),
                                                             back_to_main_menu.getRadius()),
                                                back_to_main_menu.getRadius())) {
                                                std::ofstream file(filename, std::ios::app);
                                                return;
                                            }
                                        } break;
                                        default: {
                                        } break;
                                    }
                                }
                                if (is_escape_pressed) {
                                    break;
                                }
                                draw_menu(window);
                                draw_blocks_of_data(window, mouse);
                                text.setString("Open the map \nconstructor");
                                text.setFillColor(sf::Color(0, 0, 0));
                                text.setPosition(constr.getPosition() + sf::Vector2f{100, 140});
                                window.draw(constr);
                                window.draw(text);

                                text.setString("Back to main menu");
                                text.setFillColor(sf::Color(0, 0, 0));
                                text.setPosition(back_to_main_menu.getPosition() + sf::Vector2f{50, 150});
                                window.draw(back_to_main_menu);
                                window.draw(text);

                                mouse.setPosition((sf::Vector2f)sf::Mouse::getPosition());
                                mouse.setFillColor(color_for_mouse);
                                window.draw(mouse);
                                window.display();
                            }
                        } else if (create_or_generate(window, (sf::Vector2f)sf::Mouse::getPosition()) == RANDOM_GENERATE) {
                            std::uniform_int_distribution<int> rand(1, 1e4);
                            list_of_data = list_of_default_data[rand(e1) % 10];
                            break;
                        } else {
                            break;
                        }
                    }
                } break;
                case sf::Event::KeyPressed: {
                    if (event.key.code == sf::Keyboard::Escape) {
                        text.setFillColor(sf::Color(2, 2, 2));
                        list_of_data.assign(5, {});
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
                    if (index != -1 && list_of_data[index].length() <= 14) {
                        if (event.text.unicode > 33 && event.text.unicode < 127) {
                            list_of_data[index].push_back(
                                (char)event.text.unicode);
                        }
                    }
                } break;
                default: {
                } break;
            }
        }
        draw_menu(window);
        draw_blocks_of_data(window, mouse);
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

Menu::CG Menu::map_creation_menu::create_or_generate(sf::RenderWindow &window, sf::Vector2f mouse) {
    if (check_pressing(mouse, create_block.getPosition(), create_block.getSize())) {
        return CREATE;
    } else if (check_pressing(mouse, random_map_block.getPosition(), random_map_block.getSize())) {
        return RANDOM_GENERATE;
    }
    return NOTHING;
}
void Menu::map_creation_menu::draw_blocks_of_data(sf::RenderWindow &window, sf::CircleShape &mouse) {
    text.setString("Create!");
    text.setFillColor(sf::Color(2, 2, 2));
    text.setPosition(50, 540);
    window.draw(create_block);
    window.draw(text);
    text.setString("Generate a random map");
    text.setFillColor(sf::Color(2, 2, 2));
    text.setPosition(650, 40);
    window.draw(random_map_block);
    window.draw(text);

    for (int i = 0; i < 5; i++) {
        window.draw(blocks_of_map_data[i]);
        if (list_of_data[i].length() != 0) {
            text.setString(list_of_data[i]);
            text.setFillColor(sf::Color(2, 2, 2));
        } else {
            text.setFillColor(input_color);
            switch (i) {
                case 0: {
                    text.setString("Enter the name of the mod");
                } break;
                case 1: {
                    text.setString("Enter your nickname");
                } break;
                case 2: {
                    text.setString("Enter the name of your map");
                } break;
                case 3: {
                    text.setString("Enter the music address");
                } break;
                case 4: {
                    text.setString("Enter the image address");
                } break;
                default: {
                } break;
            }
        }

        text.setPosition(10, (float)i * 100);
        window.draw(text);
        mouse.setPosition((sf::Vector2f)sf::Mouse::getPosition());
        mouse.setFillColor(sf::Color(241, 200, 14));
        window.draw(mouse);
    }
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
        mod_blocks[i].setPosition(sf::Vector2f(900, 70 * (i + 1)));
        mod_blocks[i].setSize(sf::Vector2f(400, 70));
        mod_blocks[i].setFillColor(sf::Color(255, 255, 255));
        mod_blocks[i].setOutlineColor(sf::Color(0, 0, 0));
        list_of_mods[i].setPosition(sf::Vector2f(905, (i + 1) * 70));
        list_of_mods[i].setFillColor(sf::Color(0, 0, 0));
    }
}

float Menu::return_acceleration(sf::Text text) {
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
    sf::CircleShape mouse(5.f);
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

            }
        }
        for (int i = 0; i < NUMBER_OF_MODS; i++) {
            window.draw(mod_blocks[i]);
            window.draw(list_of_mods[i]);
            mouse.setPosition((sf::Vector2f)sf::Mouse::getPosition());
            mouse.setFillColor(color_for_mouse);
            window.draw(mouse);
        }
        window.display();
    }


}

bool Menu::check_color(sf::Text text) {
    return text.getFillColor() == input_color;
}

int Menu::get_id(std::vector<sf::RectangleShape> &blocks, sf::Vector2f mouse) {
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

