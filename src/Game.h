#pragma once
#include <SFML/Graphics.hpp>
#include "Paddle.h"
#include "Ball.h"
#include "AiPaddle.h" 
enum class State { Menu, Playing, Paused, GameOver };

struct RoundedRect : sf::Drawable
{
private:
    sf::RectangleShape bg;
    sf::CircleShape    corners[4];
    float              radius = 12.f;

public:
    RoundedRect()
    {
        bg.setSize(sf::Vector2f(1, 1));
        bg.setPosition(sf::Vector2f(0, 0));
        for (int i = 0; i < 4; ++i)
        {
            corners[i].setRadius(1);
            corners[i].setPointCount(3);
            corners[i].setFillColor(sf::Color::Transparent);
        }
    }

    // ② 事后初始化（替代原来的带参构造）
    void init(sf::Vector2f size, sf::Color fill)
    {
        bg.setSize(size - sf::Vector2f(radius * 2, radius * 2));
        bg.setPosition(sf::Vector2f(radius, radius));
        bg.setFillColor(fill);

        for (int i = 0; i < 4; ++i)
        {
            corners[i].setRadius(radius);
            corners[i].setPointCount(12);
            corners[i].setFillColor(fill);
        }
        corners[0].setPosition(sf::Vector2f(0, 0));
        corners[1].setPosition(sf::Vector2f(size.x - radius, 0));
        corners[2].setPosition(sf::Vector2f(0, size.y - radius));
        corners[3].setPosition(sf::Vector2f(size.x - radius, size.y - radius));
    }

    void draw(sf::RenderTarget& tgt, sf::RenderStates s) const override
    {
        tgt.draw(bg, s);
        for (auto& c : corners) tgt.draw(c, s);
    }

    void setPosition(sf::Vector2f pos)
    {
        bg.setPosition(pos + sf::Vector2f(radius, radius));
        corners[0].setPosition(pos);
        corners[1].setPosition(pos + sf::Vector2f(bg.getSize().x + radius, 0.f));
        corners[2].setPosition(pos + sf::Vector2f(0.f, bg.getSize().y + radius));
        corners[3].setPosition(pos + sf::Vector2f(bg.getSize().x + radius, bg.getSize().y + radius));
    }
};
struct Button
{
    sf::RectangleShape rect;
    std::optional<sf::Text> text;
    bool               hovered = false;

    // ① 默认构造：空字符串 + 最小字号（字体后续用 init 填）
    Button() = default;

    // ② 事后初始化
    void init(const sf::Font& font, const std::string& label, unsigned charSize)
    {
        text.emplace(font, label, charSize); // 这里才绑定字体
    }

    bool click(const sf::Vector2f& mp) const
    {
        return rect.getGlobalBounds().contains(mp);
    }
};
class Game {
public:
    Game(unsigned width, unsigned height, const std::string& title);

    // 主循环
    void run();
    std::optional<sf::Text> m_helpText;
    // 状态切换
    void handleEvent(const sf::Event& ev);
    void update(sf::Time dt);
    void draw(sf::Time dt);

    State state() const { return m_state; }
    void setState(State s) { m_state = s; }

    // 得分接口（先留空，后续加 Ball/Paddle 再实现）
    void leftScore();
    void rightScore();

	void scoreBlink()// 闪烁计时重置
    {
        m_scoreBlink = 0.f;
    }
    void newMatch();   // 新开一局（清零分数+倒计时）

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

    // Menu visuals
    std::optional<RoundedRect> m_menuCard; // 圆角卡片
	int m_fadeAlpha = 0;// 渐变遮罩透明度
    std::optional<sf::Text> m_titleText;      // 大标题
    std::vector<std::optional<Button>> m_menuBtns;   // 开始/退出 按钮

    // 得分
    int  m_leftScore = 0;
    int  m_rightScore = 0;
    std::optional<sf::Text> m_leftScoreText;
    std::optional<sf::Text> m_rightScoreText;
    float m_scoreBlink = 0.f;   // 闪烁计时

    int  m_countDown = 60;                // 秒
    std::optional<sf::Text> m_timeText;   // 倒计时文字

    int  m_serveCount = 3;          // 发球倒计时 3→0
    std::optional<sf::Text> m_serveText; // 大数字
    bool  m_serving = true;         // 是否处于发球倒计时
    float m_serveTimer = 0.f;       // 内部计时

    // ===== AI =====

    bool m_aiEnabled = false;               // 开关
    
    std::unique_ptr<AiPaddle> m_ai;   // 指针延迟构造，避免 Paddle 前向引用


    sf::RenderWindow m_window;
    State            m_state = State::Menu;
    std::optional<sf::Font>  m_font;
    std::optional<sf::Text>  m_menuText;
    std::optional<sf::Text>  m_pauseText;
    std::optional<sf::Text>  m_gameOverText;
};




