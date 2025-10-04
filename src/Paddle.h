#pragma once
#include <SFML/Graphics.hpp>

class Paddle : public sf::RectangleShape {
public:
    Paddle(float x, float y, float width, float height, bool isLeft);

    // ÿ֡����
    void update(float dt,bool isLeft);
    // ���ƣ�RectangleShape �Դ� draw��
    void draw(sf::RenderWindow& window);

    // ���������� Y / ���
    float centerY() const;
    float halfHeight() const;

private:
    float speed = 500.f;   // ����/��
    bool m_isLeft = false;   // ��Ա���
};