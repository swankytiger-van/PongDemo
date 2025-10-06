#pragma once
#include <SFML/Graphics.hpp>

class Paddle : public sf::Drawable              
{
public:
    Paddle(float x, float y, float width, float height, bool isLeft);

    void update(float dt, bool isLeft);  
    void setPosition(sf::Vector2f pos);
    void draw(sf::RenderWindow& window);       // �Լ�����Բ

    // ԭ����Щ����
    float centerY() const;
    float halfHeight() const;
    sf::FloatRect getGlobalBounds() const;     // �ṩ AABB ���ⲿ��ײ

private:
    virtual void draw(sf::RenderTarget& tgt, sf::RenderStates states) const override;

    sf::CircleShape m_shape;   // ��Բ���
    float m_halfWidth;         // ���������� AABB
    float m_halfHeight;
    float speed = 500.f;
    bool m_isLeft = false;
};