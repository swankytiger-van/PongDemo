#include "Ball.h"
#include <iostream>

Ball::Ball(float x, float y, float radius)
    : sf::CircleShape(radius) {
    setPosition(sf::Vector2f(x, y));
    setFillColor(sf::Color::White);
    setRadius(radius);
    setOrigin(sf::Vector2f(radius, radius));   // 中心原点
}

// AABB 相交函数（几何通用工具）
bool Ball::intersect(const sf::FloatRect& a, const sf::FloatRect& b) {
    return a.position.x < b.position.x + b.size.x &&
        a.position.x + a.size.x > b.position.x &&
        a.position.y < b.position.y + b.size.y &&
        a.position.y + a.size.y > b.position.y;
}

// 每帧更新：移动 + 边界反弹
void Ball::update(float dt) {
    prevPos = getPosition();
    if (collideCooldown > 0) --collideCooldown;
    move(velocity * dt);

    // 上下边界反弹
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

    // 左右边界反弹 + 得分重置（零额外文件）
    float x = getPosition().x;
    float winW = 800.f;   // 后续放 Constants.h
    if (x - r < 0.f) {
        // 右拍得分 + 重置
        velocity.x = -velocity.x;
        setPosition(sf::Vector2f(winW / 2.f, winH / 2.f));
        velocity.y = 0.f;   // 水平发球
        std::cout << "[Ball] left out → right score + reset\n";
    }
    if (x + r > winW) {
        // 左拍得分 + 重置
        velocity.x = -velocity.x;
        setPosition(sf::Vector2f(winW / 2.f, winH / 2.f));
        velocity.y = 0.f;   // 水平发球
        std::cout << "[Ball] right out → left score + reset\n";
    }
}

void Ball::draw(sf::RenderWindow& window) {
    window.draw(*this);
}

// 辅助：中心 / 半宽 / 半高 / AABB
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

// 碰撞响应：根据击中位置给角度
void Ball::hitPaddle(float paddleCenterY, float paddleHalfHeight) {
    // 根据击中位置给角度
    float delta = (centerY() - paddleCenterY) / paddleHalfHeight;   // [-1, 1]
    velocity.y = delta * 400.f;                                     // 可调角度
    velocity.x = -velocity.x;                                       // 水平反向

}