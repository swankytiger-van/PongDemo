#include "Paddle.h"
#include <iostream>
// 构造：位置、尺寸、颜色
Paddle::Paddle(float x, float y, float width, float height, bool isLeft)
    : m_halfWidth(width * 0.5f), m_halfHeight(height * 0.5f), m_isLeft(isLeft)
{
    m_shape.setRadius(m_halfHeight);          // 半径 = 半高
    m_shape.setPointCount(30);                // 圆滑
    m_shape.setOrigin(sf::Vector2f(m_halfHeight, m_halfHeight));
    // 位置用中心坐标
    m_shape.setPosition(sf::Vector2f(x + m_halfWidth, y + m_halfHeight));
    m_shape.setFillColor(m_isLeft ? sf::Color::Red : sf::Color::Green);
}
// 每帧更新：读键盘，移动
void Paddle::update(float dt, bool isLeft)
{
    float dy = 0.f;
    if (isLeft) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) dy -= speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) dy += speed;
    }
    else {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))   dy -= speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) dy += speed;
    }
    sf::Vector2f pos = m_shape.getPosition();
    pos.y += dy * dt;

    // 边界 clamp（用半高）
    float topLim = m_halfHeight;
    float botLim = 600.f - m_halfHeight;
    pos.y = std::clamp(pos.y, topLim, botLim);
    m_shape.setPosition(pos);
}


void Paddle::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(m_shape, states);
}
float Paddle::centerY() const { return m_shape.getPosition().y; }
float Paddle::halfHeight() const { return m_halfHeight; }
sf::FloatRect Paddle::getGlobalBounds() const
{
    return sf::FloatRect(
        sf::Vector2f(m_shape.getPosition().x - m_halfWidth,
            m_shape.getPosition().y - m_halfHeight),
        sf::Vector2f(m_halfWidth * 2.f, m_halfHeight * 2.f)
    );
}