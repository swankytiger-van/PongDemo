#pragma once
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>   

class Paddle;   // 前向声明
class Ball;     // 前向声明

class AiPaddle final {
public:
    // 难度 0~1  0=木头 1=神仙
    explicit AiPaddle(float difficulty = 0.75f);

    // 每帧调用：根据球和自身位置更新 paddle
    void update(Paddle& paddle, const Ball& ball, sf::Time dt);

    // 允许运行时改难度（菜单滑条）
    void setDifficulty(float d) { m_difficulty = std::clamp(d, 0.f, 1.f); }

private:
    float m_difficulty;
    // 跟踪平滑用
    float m_targetY = 0.f;
};