#include "Ball.h"
#include <iostream>

Ball::Ball(float x, float y, float radius)
    : sf::CircleShape(radius) {
    setPosition(sf::Vector2f(x, y));
    setFillColor(sf::Color::White);
    setRadius(radius);
    setOrigin(sf::Vector2f(radius, radius));   // ����ԭ��
}

// AABB �ཻ����������ͨ�ù��ߣ�
bool Ball::intersect(const sf::FloatRect& a, const sf::FloatRect& b) {
    return a.position.x < b.position.x + b.size.x &&
        a.position.x + a.size.x > b.position.x &&
        a.position.y < b.position.y + b.size.y &&
        a.position.y + a.size.y > b.position.y;
}

// ÿ֡���£��ƶ� + �߽練��
void Ball::update(float dt)
{
    prevPos = getPosition();
    
    
    move(velocity * dt);

    // ���±߽練����������
    float y = getPosition().y;
    float r = getRadius();
    float winH = 600.f;
    if (y - r < 0.f) {
        velocity.y = -velocity.y;
        setPosition(sf::Vector2f(getPosition().x, r));
    }
    if (y + r > winH) {
        velocity.y = -velocity.y;
        setPosition(sf::Vector2f(getPosition().x, winH - r));
    }

    // ���ҳ��� �� ֻ���棬������
    float x = getPosition().x;
    float winW = 800.f;
    if (x - r < 0.f && m_outOfBounds)      // �����
    {
        m_outOfBounds(true);
        serveImmunity = 1; // ��������������ײ,��ֹreset��ʱ�������ӿ�ס��BUG
        std::cout << "serveImmunity = 1" << std::endl;
    }
        
    if (x + r > winW && m_outOfBounds)     // �ҳ���
    {
        m_outOfBounds(false);
        serveImmunity = 1; // ��������������ײ,��ֹreset��ʱ�������ӿ�ס��BUG
        std::cout << "serveImmunity = 1" << std::endl;
    }
}

void Ball::draw(sf::RenderWindow& window) {
    window.draw(*this);
}

// ���������� / ��� / ��� / AABB
float Ball::centerX() const {
    return getPosition().x;
}
float Ball::centerY() const {
    return getPosition().y;
}
float Ball::halfWidth() const {
    return getRadius();
}
float Ball::halfHeight() const {
    return getRadius();
}
sf::FloatRect Ball::getAABB() const {
    return getGlobalBounds();
}

// ��ײ��Ӧ�����ݻ���λ�ø��Ƕ�
void Ball::hitPaddle(float paddleCenterY, float paddleHalfHeight) {
    // ���ݻ���λ�ø��Ƕ�
    float delta = (centerY() - paddleCenterY) / paddleHalfHeight;   // [-1, 1]
    velocity.y = delta * 400.f;                                     // �ɵ��Ƕ�
    velocity.x = -velocity.x;                                       // ˮƽ����

}