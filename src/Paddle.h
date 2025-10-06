#pragma once
#include <SFML/Graphics.hpp>

class Paddle : public sf::Drawable              
{
public:
    Paddle(float x, float y, float width, float height, bool isLeft);

    void update(float dt, bool isLeft);  
    void setPosition(sf::Vector2f pos);
    void draw(sf::RenderWindow& window);       // 自己画半圆

    // 原来那些辅助
    float centerY() const;
    float halfHeight() const;
    sf::FloatRect getGlobalBounds() const;     // 提供 AABB 给外部碰撞

private:
    virtual void draw(sf::RenderTarget& tgt, sf::RenderStates states) const override;

    sf::CircleShape m_shape;   // 半圆外观
    float m_halfWidth;         // 保存半宽，用于 AABB
    float m_halfHeight;
    float speed = 500.f;
    bool m_isLeft = false;
};