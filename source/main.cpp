#include <cassert>
#include "SFML/Audio.hpp"
#include "SFML/Graphics.hpp"
#include "windows.h"

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

    buffer.openFromFile("..\\data\\music\\helltaker_rem.ogg");

    buffer.play();

    CircleShape circle(radius);

    circle.setFillColor(Color(230, 155, 230));
    Texture img;
    img.loadFromFile("..\\data\\img\\lucifer.png");

    RectangleShape rect(Vector2f(WIDTH, HEIGHT));
    rect.setPosition(0, 0);
    rect.setTexture(&img);
    Event event;
    while (window.isOpen()) {
        while (window.pollEvent(event)) {
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
                    circle.setRadius(radius);
                    circle.setPosition(
                        static_cast<float>(position_mouse.x) - radius,
                        static_cast<float>(position_mouse.y) - radius);
                }
            }
        }
        //        window.clear();

        // rect.setFillColor(Color(230, 0, 230));
        window.draw(rect);
        float dif = 5;
        if (circle.getRadius() > 50) {
            circle.setRadius(circle.getRadius() - dif);
            circle.setPosition(
                static_cast<float>(circle.getPosition().x) + dif,
                static_cast<float>(circle.getPosition().y) + dif);
            window.draw(circle);
        }
        window.display();

        //        Sleep(100);
    }
    return 0;
}