#include "AiPaddle.h"
#include "Paddle.h"
#include "Ball.h"
#include <algorithm>
#include <cmath>
constexpr float WINDOW_H = 600.f;
namespace {
    // ����Ԥ���򵽴� paddleX ʱ�� Y ֵ������ǽ������
    float predictBallY(const Ball& ball, float paddleX, float paddleH) {
        const sf::Vector2f pos = ball.getPosition();
        const sf::Vector2f vel = ball.velocity;

        if (std::fabs(vel.x) < 1e-6f) return pos.y;          // ��ֱ��
        const float t = std::fabs((paddleX - pos.x) / vel.x);
        float y = pos.y + vel.y * t;

        // ������ǽ���޷���
        constexpr float H = 600.f;
        const int bounces = static_cast<int>(std::fabs(y) / H);
        y = std::fmod(std::fabs(y), H);
        if (bounces & 1) y = H - y;              // �����ξ���
        return std::clamp(y, 0.f, H - paddleH);
    }
} // namespace

AiPaddle::AiPaddle(float difficulty) : m_difficulty(std::clamp(difficulty, 0.f, 1.f)) {}

void AiPaddle::update(Paddle& paddle, const Ball& ball, sf::Time dt) {
    // 1. �õ�Ŀ�� Y��Ԥ�� or ֱ�Ӹ��ٿ��л���
    const float paddleW = paddle.getGlobalBounds().size.x;
    const float paddleH = paddle.getGlobalBounds().size.y;
    const float paddleX = paddle.getPosition().x;
    // ������Ԥ���㷨�����˻��ɡ����١��Ͱ� predictBallY ���� ball.getPosition().y
    m_targetY = predictBallY(ball, paddleX, paddleH);

    // 2. ��ǰ����
    const float curY = paddle.centerY();

    // 3. �����ƶ�
    float dy = m_targetY - curY;
    float maxSpeed = 420.f * m_difficulty;          // �Ѷ�ֱ�ӳ��ٶ�
    float move = std::copysign(std::min(std::fabs(dy), maxSpeed * dt.asSeconds()), dy);

    float newY = paddle.getPosition().y + move;
    float topLim = paddle.halfHeight();            // �ϱ߽�
    float botLim = 600.f - paddle.halfHeight();    // �±߽�
    newY = std::clamp(newY, topLim, botLim);

    // 4. Ӧ��
    paddle.setPosition(sf::Vector2f(paddle.getPosition().x, newY));
}