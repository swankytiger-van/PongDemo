#pragma once
#include <SFML/Graphics.hpp>

class Paddle : public sf::RectangleShape {
public:
    Paddle(float x, float y, float width, float height, bool isLeft);

    // 每帧更新
    void update(float dt,bool isLeft);
    // 绘制（RectangleShape 自带 draw）
    void draw(sf::RenderWindow& window);

    // 辅助：中心 Y / 半高
    float centerY() const;
    float halfHeight() const;

private:
    float speed = 500.f;   // 像素/秒
    bool m_isLeft = false;   // 成员标记
};