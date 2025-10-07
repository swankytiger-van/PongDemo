#pragma once
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>   

class Paddle;   // ǰ������
class Ball;     // ǰ������

class AiPaddle final {
public:
    // �Ѷ� 0~1  0=ľͷ 1=����
    explicit AiPaddle(float difficulty = 0.75f);

    // ÿ֡���ã������������λ�ø��� paddle
    void update(Paddle& paddle, const Ball& ball, sf::Time dt);

    // ��������ʱ���Ѷȣ��˵�������
    void setDifficulty(float d) { m_difficulty = std::clamp(d, 0.f, 1.f); }

private:
    float m_difficulty;
    // ����ƽ����
    float m_targetY = 0.f;
};