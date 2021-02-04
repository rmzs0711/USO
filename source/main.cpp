#include <cassert>
#include "SFML/Audio.hpp"
#include "SFML/Graphics.hpp"

using namespace sf;

int main() {
    ContextSettings setting;

    setting.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(1080, 720), "SFML Works!",
                            Style::Fullscreen, setting);
    sf::Vector2i position_circle{0, 0};
    sf::Vector2i position_mouse{0, 0};
    const float radius = 100;
    const unsigned HEIGHT = sf::VideoMode::getFullscreenModes().front().height;
    const unsigned WIDTH = sf::VideoMode::getFullscreenModes().front().width;
    Music buffer;

    buffer.openFromFile(R"(..\data\helltaker_rem.ogg)");

    buffer.play();

    while (window.isOpen()) {
        Event event;
        Texture img;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
            if (event.type == Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    position_mouse = Mouse::getPosition();
                }
            }
        }

        img.loadFromFile("..\\data\\lucifer.png");

        RectangleShape rect(Vector2f(WIDTH, HEIGHT));
        //        rect.setFillColor(Color(230, 0, 230));
        rect.setPosition(0, 0);
        rect.setTexture(&img);

        CircleShape circle(radius);
        circle.setPosition(static_cast<float>(position_mouse.x - 100),
                           static_cast<float>(position_mouse.y - 100));
        circle.setFillColor(Color(230, 155, 230));

        window.draw(rect);
        window.draw(circle);

        window.display();
    }
    return 0;
}