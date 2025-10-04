#include "Paddle.h"
#include <iostream>
// 构造：位置、尺寸、颜色
Paddle::Paddle(float x, float y, float width, float height, bool isLeft)
    : m_isLeft(isLeft) {
    setPosition(sf::Vector2f(x, y));
    setSize(sf::Vector2f(width, height));
    setFillColor(sf::Color::White);
}

// 每帧更新：读键盘，移动
void Paddle::update(float dt, bool isLeft) {
    float dy = 0.f;
    if (isLeft) {
        // 左拍：W/S
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) dy -= speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) dy += speed;
        //std::cout << "[Left Paddle] new y = " << getPosition().y << std::endl;
    }
    else {
        // 右拍：↑/↓
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) dy -= speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) dy += speed;
        //std::cout << "[Right Paddle] new y = " << getPosition().y << std::endl;
    }
    move(sf::Vector2f(0.f, dy * dt));

    // 窗口边界 clamp（业务逻辑）
    float y = getPosition().y;
    float h = getSize().y;
    float winH = 600.f;   // 后续放 Constants.h
    float newY = std::clamp(y, 0.f, winH - h);
    if (newY != y) {
        std::cout << "[Paddle] clamped to " << newY << std::endl;
    }
    setPosition(sf::Vector2f(getPosition().x, newY));
}


void Paddle::draw(sf::RenderWindow& window) {
    window.draw(*this);   // RectangleShape 自带 draw
}

float Paddle::centerY() const {
    return getPosition().y + getSize().y * 0.5f;
}

float Paddle::halfHeight() const {
    return getSize().y * 0.5f;
}