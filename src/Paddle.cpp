#include "Paddle.h"
#include <iostream>
// ���죺λ�á��ߴ硢��ɫ
Paddle::Paddle(float x, float y, float width, float height, bool isLeft)
    : m_isLeft(isLeft) {
    setPosition(sf::Vector2f(x, y));
    setSize(sf::Vector2f(width, height));
    setFillColor(sf::Color::White);
}

// ÿ֡���£������̣��ƶ�
void Paddle::update(float dt, bool isLeft) {
    float dy = 0.f;
    if (isLeft) {
        // ���ģ�W/S
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) dy -= speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) dy += speed;
        //std::cout << "[Left Paddle] new y = " << getPosition().y << std::endl;
    }
    else {
        // ���ģ���/��
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) dy -= speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) dy += speed;
        //std::cout << "[Right Paddle] new y = " << getPosition().y << std::endl;
    }
    move(sf::Vector2f(0.f, dy * dt));

    // ���ڱ߽� clamp��ҵ���߼���
    float y = getPosition().y;
    float h = getSize().y;
    float winH = 600.f;   // ������ Constants.h
    float newY = std::clamp(y, 0.f, winH - h);
    if (newY != y) {
        std::cout << "[Paddle] clamped to " << newY << std::endl;
    }
    setPosition(sf::Vector2f(getPosition().x, newY));
}


void Paddle::draw(sf::RenderWindow& window) {
    window.draw(*this);   // RectangleShape �Դ� draw
}

float Paddle::centerY() const {
    return getPosition().y + getSize().y * 0.5f;
}

float Paddle::halfHeight() const {
    return getSize().y * 0.5f;
}