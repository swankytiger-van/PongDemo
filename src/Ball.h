#pragma once
#include <SFML/Graphics.hpp>
#include <functional>  

class Ball : public sf::CircleShape {
public:
    Ball(float x, float y, float radius);
    using OutOfBoundsCallback = std::function<void(bool isLeft)>;
    // �󶨳���ص���Game ����ã�
    void setOutOfBoundsCallback(OutOfBoundsCallback cb) { m_outOfBounds = std::move(cb); }
    static bool intersect(const sf::FloatRect& a, const sf::FloatRect& b);
    sf::Vector2f prevPos;
    int collideCooldown = 0;
    // ÿ֡����
    void update(float dt);
    // ���ƣ�CircleShape �Դ� draw��
    void draw(sf::RenderWindow& window);

    // ���������� X/Y / ��� / ��� / AABB
    float centerX() const;
    float centerY() const;
    float halfWidth() const;
    float halfHeight() const;
    sf::FloatRect getAABB() const;
    sf::Vector2f velocity{ 400.f, 0.f };   // ����/��
    // ��ײ��Ӧ
    void hitPaddle(float paddleCenterY, float paddleHalfHeight);
	int serveImmunity = 0;// ���Ƿ�������ײ

private:
    
    float speed = 500.f;                 // �ɵ��ٶ�
	OutOfBoundsCallback m_outOfBounds; // ����ص�
};