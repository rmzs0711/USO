#include <cassert>
#include "SFML/Audio.hpp"
#include "SFML/Graphics.hpp"
#include "map_objects.h"
#include "windows.h"
#include <random>

using namespace sf;

int main() {
    ContextSettings setting;

    setting.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(1080, 720), "SFML Works!",
                            Style::Fullscreen, setting);
    sf::Vector2i position_circle{0, 0};
    sf::Vector2i position_mouse{0, 0};
    window.setVerticalSyncEnabled(true);
    const float radius = 200;
    const unsigned HEIGHT = sf::VideoMode::getFullscreenModes().front().height;
    const unsigned WIDTH = sf::VideoMode::getFullscreenModes().front().width;
    Music buffer;

    //    buffer.openFromFile(R"(data\music\helltaker_rem.ogg)");
    //
    //    buffer.play();

    CircleShape circle(radius);

    circle.setFillColor(Color(230, 155, 230));
    Texture img;
    img.loadFromFile(R"(data\img\lucifer.png)");

    RectangleShape rect(Vector2f(WIDTH, HEIGHT));
    rect.setPosition(0, 0);
    rect.setTexture(&img);
    sf::Time time1 = sf::seconds(0);
    sf::Time time2 = sf::seconds(1);
    sf::Time move_time = sf::seconds(5);
    BL::Game_session gameSession;
    sf::Clock timer;
    USO::Aim_circle aimCircle(time1, time2, 100.0, 100, 1, 100, 300);
    while (window.isOpen()) {
        Event event{};
        if (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
            if (event.type == Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    position_mouse = Mouse::getPosition();
//                    circle.setRadius(radius);
//                    circle.setPosition(
//                        static_cast<float>(position_mouse.x) - radius,
//                        static_cast<float>(position_mouse.y) - radius);
                    if (aimCircle.check_event({static_cast<float>(position_mouse.x),
                                               static_cast<float>(position_mouse.y)},
                                              gameSession, timer.getElapsedTime())) {
                        aimCircle = USO::Aim_circle(timer.getElapsedTime(), time2, rand() % WIDTH, rand() % HEIGHT, 1, 100, 300);
                    }
                }
            }
        }
        //        window.clear();

        aimCircle.change_state(timer.getElapsedTime());
        // rect.setFillColor(Color(230, 0, 230));
        window.draw(rect);
        aimCircle.draw(window);
//        float dif = 20;
        if (circle.getRadius() > 50) {
//            circle.setRadius(circle.getRadius() - dif);
//            circle.setPosition(
//                static_cast<float>(circle.getPosition().x) + dif,
//                static_cast<float>(circle.getPosition().y) + dif);
//            window.draw(circle);
        }
        window.display();

        //        Sleep(100);
    }
    return 0;
}