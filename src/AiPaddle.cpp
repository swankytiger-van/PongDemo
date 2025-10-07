#include "AiPaddle.h"
#include "Paddle.h"
#include "Ball.h"
#include <algorithm>
#include <cmath>
constexpr float WINDOW_H = 600.f;
namespace {
    // 线性预测球到达 paddleX 时的 Y 值（考虑墙反弹）
    float predictBallY(const Ball& ball, float paddleX, float paddleH) {
        const sf::Vector2f pos = ball.getPosition();
        const sf::Vector2f vel = ball.velocity;

        if (std::fabs(vel.x) < 1e-6f) return pos.y;          // 垂直球
        const float t = std::fabs((paddleX - pos.x) / vel.x);
        float y = pos.y + vel.y * t;

        // 简化上下墙无限反弹
        constexpr float H = 600.f;
        const int bounces = static_cast<int>(std::fabs(y) / H);
        y = std::fmod(std::fabs(y), H);
        if (bounces & 1) y = H - y;              // 奇数次镜像
        return std::clamp(y, 0.f, H - paddleH);
    }
} // namespace

AiPaddle::AiPaddle(float difficulty) : m_difficulty(std::clamp(difficulty, 0.f, 1.f)) {}

void AiPaddle::update(Paddle& paddle, const Ball& ball, sf::Time dt) {
    // 1. 拿到目标 Y（预测 or 直接跟踪可切换）
    const float paddleW = paddle.getGlobalBounds().size.x;
    const float paddleH = paddle.getGlobalBounds().size.y;
    const float paddleX = paddle.getPosition().x;
    // 这里用预测算法，想退化成“跟踪”就把 predictBallY 换成 ball.getPosition().y
    m_targetY = predictBallY(ball, paddleX, paddleH);

    // 2. 当前中心
    const float curY = paddle.centerY();

    // 3. 计算移动
    float dy = m_targetY - curY;
    float maxSpeed = 420.f * m_difficulty;          // 难度直接乘速度
    float move = std::copysign(std::min(std::fabs(dy), maxSpeed * dt.asSeconds()), dy);

    float newY = paddle.getPosition().y + move;
    float topLim = paddle.halfHeight();            // 上边界
    float botLim = 600.f - paddle.halfHeight();    // 下边界
    newY = std::clamp(newY, topLim, botLim);

    // 4. 应用
    paddle.setPosition(sf::Vector2f(paddle.getPosition().x, newY));
}