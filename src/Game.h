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

    // �� �º��ʼ�������ԭ���Ĵ��ι��죩
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

    // �� Ĭ�Ϲ��죺���ַ��� + ��С�ֺţ���������� init �
    Button() = default;

    // �� �º��ʼ��
    void init(const sf::Font& font, const std::string& label, unsigned charSize)
    {
        text.emplace(font, label, charSize); // ����Ű�����
    }

    bool click(const sf::Vector2f& mp) const
    {
        return rect.getGlobalBounds().contains(mp);
    }
};
class Game {
public:
    Game(unsigned width, unsigned height, const std::string& title);

    // ��ѭ��
    void run();
    std::optional<sf::Text> m_helpText;
    // ״̬�л�
    void handleEvent(const sf::Event& ev);
    void update(sf::Time dt);
    void draw(sf::Time dt);

    State state() const { return m_state; }
    void setState(State s) { m_state = s; }

    // �÷ֽӿڣ������գ������� Ball/Paddle ��ʵ�֣�
    void leftScore();
    void rightScore();

	void scoreBlink()// ��˸��ʱ����
    {
        m_scoreBlink = 0.f;
    }
    void newMatch();   // �¿�һ�֣��������+����ʱ��

private:
    void reset();   // һ������
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
    std::optional<RoundedRect> m_menuCard; // Բ�ǿ�Ƭ
	int m_fadeAlpha = 0;// ��������͸����
    std::optional<sf::Text> m_titleText;      // �����
    std::vector<std::optional<Button>> m_menuBtns;   // ��ʼ/�˳� ��ť

    // �÷�
    int  m_leftScore = 0;
    int  m_rightScore = 0;
    std::optional<sf::Text> m_leftScoreText;
    std::optional<sf::Text> m_rightScoreText;
    float m_scoreBlink = 0.f;   // ��˸��ʱ

    int  m_countDown = 60;                // ��
    std::optional<sf::Text> m_timeText;   // ����ʱ����

    int  m_serveCount = 3;          // ���򵹼�ʱ 3��0
    std::optional<sf::Text> m_serveText; // ������
    bool  m_serving = true;         // �Ƿ��ڷ��򵹼�ʱ
    float m_serveTimer = 0.f;       // �ڲ���ʱ

    // ===== AI =====

    bool m_aiEnabled = false;               // ����
    
    std::unique_ptr<AiPaddle> m_ai;   // ָ���ӳٹ��죬���� Paddle ǰ������


    sf::RenderWindow m_window;
    State            m_state = State::Menu;
    std::optional<sf::Font>  m_font;
    std::optional<sf::Text>  m_menuText;
    std::optional<sf::Text>  m_pauseText;
    std::optional<sf::Text>  m_gameOverText;
};




