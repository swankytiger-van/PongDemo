#include <SFML/Graphics.hpp>

int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({800u, 600u}), "CMake SFML Project");
    window.setFramerateLimit(144);

    sf::CircleShape shape(50.f);          // �뾶 50 ����
    shape.setFillColor(sf::Color::Green);
    shape.setPosition({400.f, 300.f});    // ʹ�� sf::Vector2f ����λ��

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        window.clear();
        window.draw(shape);
        window.display();
    }
}
