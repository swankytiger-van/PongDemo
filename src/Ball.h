#pragma once
#include <SFML/Graphics.hpp>
#include <functional>  

class Ball : public sf::CircleShape {
public:
    Ball(float x, float y, float radius);
    using OutOfBoundsCallback = std::function<void(bool isLeft)>;
    // 绑定出界回调（Game 会调用）
    void setOutOfBoundsCallback(OutOfBoundsCallback cb) { m_outOfBounds = std::move(cb); }
    static bool intersect(const sf::FloatRect& a, const sf::FloatRect& b);
    sf::Vector2f prevPos;
    int collideCooldown = 0;
    // 每帧更新
    void update(float dt);
    // 绘制（CircleShape 自带 draw）
    void draw(sf::RenderWindow& window);

    // 辅助：中心 X/Y / 半宽 / 半高 / AABB
    float centerX() const;
    float centerY() const;
    float halfWidth() const;
    float halfHeight() const;
    sf::FloatRect getAABB() const;
    sf::Vector2f velocity{ 400.f, 0.f };   // 像素/秒
    // 碰撞响应
    void hitPaddle(float paddleCenterY, float paddleHalfHeight);
	int serveImmunity = 0;// 球是否免疫碰撞

private:
    
    float speed = 500.f;                 // 可调速度
	OutOfBoundsCallback m_outOfBounds; // 出界回调
};