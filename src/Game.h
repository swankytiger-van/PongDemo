#pragma once
#include <SFML/Graphics.hpp>
#include "Paddle.h"
#include "Ball.h"
enum class State { Menu, Playing, Paused, GameOver };

class Game {
public:
    Game(unsigned width, unsigned height, const std::string& title);

    // 主循环
    void run();

    // 状态切换
    void handleEvent(const sf::Event& ev);
    void update(sf::Time dt);
    void draw(sf::Time dt);

    State state() const { return m_state; }
    void setState(State s) { m_state = s; }

    // 得分接口（先留空，后续加 Ball/Paddle 再实现）
    void leftScore() { /*TODO*/ }
    void rightScore() { /*TODO*/ reset(); }

private:
    void reset();   // 一局重置
    void menuUpdate(sf::Time dt);
    void playingUpdate(sf::Time dt);
    void pausedUpdate(sf::Time dt);
    void gameOverUpdate(sf::Time dt);

    void menuDraw();
    void playingDraw(sf::Time dt);
    void pausedDraw(sf::Time dt);
    void gameOverDraw();

    std::optional<Paddle> m_leftPaddle;
    std::optional<Paddle> m_rightPaddle;
    std::optional<sf::Text> m_fpsText;
    std::optional<sf::Text> m_ballText;

    std::optional<Ball> m_ball;

    sf::RenderWindow m_window;
    State            m_state = State::Menu;
    std::optional<sf::Font>  m_font;
    std::optional<sf::Text>  m_menuText;
    std::optional<sf::Text>  m_pauseText;
    std::optional<sf::Text>  m_gameOverText;
};
