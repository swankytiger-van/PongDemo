#include "Game.h"
#include <SFML/Graphics.hpp>
#include <SFML/Config.hpp>
#include <iostream>
#include <cmath> 
#include <SFML/Window/Event.hpp>
#include <cstdint>
struct VersionCheck {
    VersionCheck() {
        std::cout << "SFML_MAJOR: " << SFML_VERSION_MAJOR
            << " SFML_MINOR: " << SFML_VERSION_MINOR << std::endl;
    }
} versionCheck;
#include <optional>
static_assert(SFML_VERSION_MAJOR >= 3, "Need SFML 3");


// ������Ӳ���룬������ Constants.h
constexpr unsigned WINDOW_W = 800;
constexpr unsigned WINDOW_H = 600;
constexpr float    DT = 1.f / 60.f; // �̶� 60 �߼�֡


// ���и���
static void centreText(sf::Text& text, float x, float y) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(sf::Vector2f(bounds.position.x + bounds.size.x * 0.5f,
        bounds.position.y + bounds.size.y * 0.5f));
    text.setPosition(sf::Vector2f(x, y));
}

//�Ż���ײ����
namespace {
    // �߶� (p0��p1) �롰���ͺ�� AABB���ཻ����
    // rect  : ԭʼ AABB
    // radius: ��뾶����������
    // t     : �����ײ���� [0,1]
    bool sweepSegmentAABB(const sf::Vector2f& p0,
        const sf::Vector2f& p1,
        const sf::FloatRect& rect,
        float radius,
        float& t)
    {
        sf::Vector2f d = p1 - p0;
        if (std::fabs(d.x) < 1e-6f && std::fabs(d.y) < 1e-6f)
            return rect.contains(p0);

        float tNear = 0.f, tFar = 1.f;

        for (int axis = 0; axis < 2; ++axis) {
            float vel = (axis == 0 ? d.x : d.y);
            float minR = (axis == 0 ? rect.position.x : rect.position.y) - radius;
            float maxR = minR + (axis == 0 ? rect.size.x : rect.size.y) + 2.f * radius;
            float pos = (axis == 0 ? p0.x : p0.y);

            if (std::fabs(vel) < 1e-6f) {
                if (pos < minR || pos > maxR) return false;
            }
            else {
                float t1 = (minR - pos) / vel;
                float t2 = (maxR - pos) / vel;
                if (t1 > t2) std::swap(t1, t2);
                tNear = std::max(tNear, t1);
                tFar = std::min(tFar, t2);
                if (tNear > tFar || tFar < 0.f) return false;
            }
        }
        t = tNear;
        return tNear <= 1.f && tNear >= 0.f;
    }
} // namespace

Game::Game(unsigned width, unsigned height, const std::string& title): m_window(sf::VideoMode({ width, height }), title)
{
    m_window.setVerticalSyncEnabled(true);

    // ������أ�SFML 3 Ψһ��ڣ�
    m_font.emplace();                       // ��Ĭ�Ϲ���
    if (!m_font->openFromFile("assets/font.ttf"))  // �ټ���
        throw std::runtime_error("Failed to open assets/font.ttf");

    // ԭλ���� Text���㿽����
    const sf::Font& font = *m_font;
    m_menuText.emplace( font,"PongCore\nSpace: Start  Esc: Exit", 24u);
    m_pauseText.emplace( font,"PAUSED\nSpace: Resume  R: Reset", 24u);
    m_gameOverText.emplace( font,"GAME OVER\nR: Restart  Esc: Exit", 24u);

    // ����˫�ģ��������Դ��
    m_leftPaddle.emplace(20.f, 250.f, 15.f, 100.f, true);   // ����
    m_rightPaddle.emplace(765.f, 250.f, 15.f, 100.f, false); // ����


    m_fpsText.emplace(*m_font, "FPS: --", 20u);
    m_fpsText->setFillColor(sf::Color::White);
    m_fpsText->setPosition(sf::Vector2f(10.f, 10.f));
    

    m_ballText.emplace(*m_font, "Ball pos: --", 20u);
    m_ballText->setFillColor(sf::Color::White);
    m_ballText->setPosition(sf::Vector2f(10.f, 35.f));

	// ������
    m_ball.emplace(400.f, 300.f, 10.f);
    m_ball->velocity = sf::Vector2f(400.f, 0.f);
    //��ʽ & λ��
    m_menuText->setFillColor(sf::Color::White);
    centreText(*m_menuText, WINDOW_W / 2.f, WINDOW_H / 2.f - 40.f);

    m_pauseText->setFillColor(sf::Color::Yellow);
    centreText(*m_pauseText, WINDOW_W / 2.f, WINDOW_H / 2.f);

    m_gameOverText->setFillColor(sf::Color::Red);
    centreText(*m_gameOverText, WINDOW_W / 2.f, WINDOW_H / 2.f);

    // �˵���Ƭ
    m_menuCard.emplace();                       // �� Ĭ�Ϲ���
    m_menuCard->init(sf::Vector2f(400, 300), sf::Color(0, 0, 0, 220)); // �� �º��ʼ��
    m_menuCard->setPosition(sf::Vector2f(WINDOW_W / 2.f - 200.f, WINDOW_H / 2.f - 150.f));


    // �����
    m_titleText.emplace(*m_font, "PONG  CORE", 60u);   // ����������Ҫ
    m_titleText->setFillColor(sf::Color(0, 255, 150));
    centreText(*m_titleText, WINDOW_W / 2.f, WINDOW_H / 2.f - 120.f);
    
    // ��ʼ/�˳� ��ť
    auto addBtn = [&](const std::string& label, float y) {
        m_menuBtns.emplace_back();                       // �ƿ� optional
        auto& btn = m_menuBtns.back().emplace();         // Ĭ�Ϲ���

        btn.rect.setSize(sf::Vector2f(200, 50));
        btn.rect.setFillColor(sf::Color(50, 50, 50));
        btn.rect.setOutlineThickness(2);
        btn.rect.setOutlineColor(sf::Color::White);
        btn.rect.setPosition(sf::Vector2f(WINDOW_W / 2.f - 100.f, y));

        btn.init(*m_font, label, 24u);                   // ����ʵ����

        if (btn.text)    // ȷ���ѹ���
            centreText(*btn.text,
                btn.rect.getPosition().x + 100.f,
                btn.rect.getPosition().y + 25.f);
        };
    addBtn("START", WINDOW_H / 2.f - 40.f);
    addBtn("EXIT", WINDOW_H / 2.f + 40.f);

    m_helpText.emplace(*m_font, "Left: W/S   Right: UP/DN   ESC: Menu   Space:Pause", 18u);
    m_helpText->setFillColor(sf::Color(200, 200, 200, 180));
    centreText(*m_helpText, WINDOW_W / 2.f, 10.f);   
}

void Game::run()
{
    sf::Clock clock;
    while (m_window.isOpen())
    {
        sf::Time dt = clock.restart();
        if (dt.asSeconds() > 0.05f) dt = sf::seconds(0.05f);

        // 1. �¼�
        while (const auto optEv = m_window.pollEvent())
        {
            handleEvent(*optEv);
        }

        // 2. �߼�
        update(dt);

        // 3. ��Ⱦ
        m_window.clear(sf::Color::Black);
        draw(dt);
        m_window.display();
    }
}


// �¼��ַ�
void Game::handleEvent(const sf::Event& ev)
{
    // 1. �˵���ť�����ֻ�ڲ˵�״̬����
    if (m_state == State::Menu && ev.is<sf::Event::MouseButtonPressed>())
    {
        auto mp = sf::Vector2f(ev.getIf<sf::Event::MouseButtonPressed>()->position);
        for (auto& opt : m_menuBtns)
        {
            if (!opt) continue;
            Button& btn = *opt;
            if (btn.click(mp))
            {
                // ����³�����
                btn.rect.setScale(sf::Vector2f(0.95f, 0.95f));

                if (btn.text->getString() == "START")
                {
                    setState(State::Playing);
                    reset();          // �¿�һ��
                }
                else if (btn.text->getString() == "EXIT")
                {
                    m_window.close();
                }
                break; // ֻ��һ��
            }
        }
        return; // �˵��¼���������ˣ������߼���
    }

    // 2. ԭ�����߼�
    if (auto kp = ev.getIf<sf::Event::KeyPressed>())
    {
        sf::Keyboard::Key key = kp->code;
        if (key == sf::Keyboard::Key::Escape)
        {
            // ���߼����˵����˳����������ز˵�
            if (m_state == State::Menu)
                m_window.close();
            else
                setState(State::Menu);
        }
        else if (key == sf::Keyboard::Key::Space)
        {
            if (m_state == State::Menu || m_state == State::Paused)
                setState(State::Playing);
            else if (m_state == State::Playing)
                setState(State::Paused);
        }
        else if (key == sf::Keyboard::Key::R)
            reset();
    }
}
// ���·ַ�
void Game::update(sf::Time dt) {
    switch (m_state) {
    case State::Menu:    
        menuUpdate(dt);    

        break;
    case State::Playing:
        // 1. ��������
        if (m_leftPaddle)   m_leftPaddle->update(dt.asSeconds(), true);
        if (m_rightPaddle)  m_rightPaddle->update(dt.asSeconds(), false);

        // 2. ������
        if (m_ball)         m_ball->update(dt.asSeconds());
        playingDraw(dt);
        // 3. ��ײ��⣨���������֮�󣬷�ֹ����ģ��,��
        if (m_ball && m_leftPaddle && m_ball->collideCooldown == 0) {
            sf::FloatRect paddleAABB = m_leftPaddle->getGlobalBounds();
            float t = 1.f;
            if (sweepSegmentAABB(m_ball->prevPos,
                m_ball->getPosition(),
                paddleAABB,
                m_ball->getRadius(),
                t))
            {
                // 1. �ع�����ײ��
                sf::Vector2f hitPos = m_ball->prevPos +
                    t * (m_ball->getPosition() - m_ball->prevPos);
                m_ball->setPosition(hitPos);

                // 2. �������淨�߰����ơ����������
                float paddleRight = paddleAABB.position.x + paddleAABB.size.x;
                float ballR = m_ball->getRadius();
                m_ball->setPosition(sf::Vector2f(paddleRight + ballR,
                    m_ball->getPosition().y));

                // 3. ����
                m_ball->hitPaddle(m_leftPaddle->centerY(),
                    m_leftPaddle->halfHeight());

                // 4. ����ȴ��֡�����⣬2~3 ֡���ɣ�
                m_ball->collideCooldown = 3;
                std::cout << "[Game] left paddle swept hit\n";
            }
        }

        // �Ҳ�����ȫ�Գ�
        if (m_ball && m_rightPaddle && m_ball->collideCooldown == 0) {
            sf::FloatRect paddleAABB = m_rightPaddle->getGlobalBounds();
            float t = 1.f;
            if (sweepSegmentAABB(m_ball->prevPos,
                m_ball->getPosition(),
                paddleAABB,
                m_ball->getRadius(),
                t))
            {
                sf::Vector2f hitPos = m_ball->prevPos +
                    t * (m_ball->getPosition() - m_ball->prevPos);
                m_ball->setPosition(hitPos);

                float paddleLeft = paddleAABB.position.x;
                float ballR = m_ball->getRadius();
                m_ball->setPosition(sf::Vector2f(paddleLeft - ballR,
                    m_ball->getPosition().y));

                m_ball->hitPaddle(m_rightPaddle->centerY(),
                    m_rightPaddle->halfHeight());
                m_ball->collideCooldown = 3;
                std::cout << "[Game] right paddle swept hit\n";
            }
        }

        break;
    
    case State::Paused:  
        pausedUpdate(dt);  

        break;

    case State::GameOver:
        gameOverUpdate(dt); 

        break;
    default:
        std::cout << "[Game] Other state\n";

        break;
    
    
    }
    // ���µ������
    static sf::Clock fpsClock;
    static int frameCount = 0;
    frameCount++;
    if (fpsClock.getElapsedTime() >= sf::seconds(0.25f)) {
        float fps = frameCount / fpsClock.getElapsedTime().asSeconds();
        if (m_fpsText)                                    
            m_fpsText->setString("FPS: " + std::to_string(static_cast<int>(fps)));
        frameCount = 0;
        fpsClock.restart();
    }
    if (m_ball && m_ballText) {                     
        sf::Vector2f pos = m_ball->getPosition();
        sf::Vector2f vel = m_ball->velocity;
        m_ballText->setString(
            "Ball pos: " + std::to_string(static_cast<int>(pos.x)) + "," +
            std::to_string(static_cast<int>(pos.y)) + "\n" +
            "Vel: " + std::to_string(static_cast<int>(vel.x)) + "," +
            std::to_string(static_cast<int>(vel.y)));

    }
    // ���뵭��
    if (m_state != State::Playing)
        m_fadeAlpha = std::min(m_fadeAlpha + 5, 200);
    else
        m_fadeAlpha = std::max(m_fadeAlpha - 5, 0);
}

// ���Ʒַ�
void Game::draw(sf::Time dt){
    m_window.clear(sf::Color::Black);   // ͳһ����
    sf::RectangleShape fade;
    fade.setSize(sf::Vector2f(WINDOW_W, WINDOW_H));
    fade.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(m_fadeAlpha)));
    m_window.draw(fade);
    switch (m_state) {
    case State::Menu:
        menuDraw();
        break;
    case State::Playing:
        playingDraw(dt);   // ���������� & ��
        break;
    case State::Paused:
        playingDraw(dt);   // ������������ֻ�ǵ�һ������
        pausedDraw(dt);
        break;
    case State::GameOver:
        playingDraw(dt);   // ��������
        gameOverDraw();
        break;
    }

    m_window.display();   // ͳһ����
}

// ��״̬��ʵ�֣��������߼�
void Game::menuUpdate(sf::Time dt)
{
    // �������⣨��ѡ��
    static float breath = 0.f;
    breath += dt.asSeconds() * 3.f;
    float scale = 1.f + std::sin(breath) * 0.05f;
    if (m_titleText)
        m_titleText->setScale(sf::Vector2f(scale, scale));

    // ��ͣ���
    sf::Vector2f mp = sf::Vector2f(sf::Mouse::getPosition(m_window));
    for (auto& opt : m_menuBtns)
    {
        if (!opt) continue;
        Button& btn = *opt;
        bool nowHover = btn.click(mp);
        if (nowHover && !btn.hovered)      // ��ͣ����
        {
            btn.rect.setFillColor(sf::Color(80, 80, 80));
            btn.rect.setScale(sf::Vector2f(1.05f, 1.05f));
        }
        else if (!nowHover && btn.hovered) // ��ͣ�뿪
        {
            btn.rect.setFillColor(sf::Color(50, 50, 50));
            btn.rect.setScale(sf::Vector2f(1.f, 1.f));
        }
        btn.hovered = nowHover;
        if (!btn.hovered) btn.rect.setScale(sf::Vector2f(1.f, 1.f)); // ���ֵ���
    }

}
void Game::playingUpdate(sf::Time dt) {
    if (m_ball && m_leftPaddle) {
        sf::FloatRect ballAABB = m_ball->getAABB();
        sf::FloatRect paddleAABB = m_leftPaddle->getGlobalBounds();
        if (Ball::intersect(ballAABB, paddleAABB)) {
            m_ball->hitPaddle(m_leftPaddle->centerY(), m_leftPaddle->halfHeight());
            std::cout << "[Game] left paddle hit\n";
        }
    }
    if (m_ball && m_rightPaddle) {
        sf::FloatRect ballAABB = m_ball->getAABB();
        sf::FloatRect paddleAABB = m_rightPaddle->getGlobalBounds();
        if (Ball::intersect(ballAABB, paddleAABB)) {
            m_ball->hitPaddle(m_rightPaddle->centerY(), m_rightPaddle->halfHeight());
            std::cout << "[Game] right paddle hit\n";
        }
    }
}
void Game::pausedUpdate(sf::Time dt) { /*TODO*/ }
void Game::gameOverUpdate(sf::Time dt) { /*TODO*/ }

void Game::menuDraw()
{
    // 1. ��Ƭ����
    m_window.draw(*m_menuCard);
    // 2. ����
    m_window.draw(*m_titleText);
    // 3. ��ť
    for (const auto& opt : m_menuBtns)
    {
        if (!opt) continue;
        const Button& btn = *opt;
        m_window.draw(btn.rect);
        if (btn.text) m_window.draw(*btn.text);
    }
}
void Game::pausedDraw(sf::Time dt) {
    playingDraw(dt);
    if (m_pauseText)  m_window.draw(*m_pauseText); 
}
void Game::gameOverDraw() { if (m_gameOverText)m_window.draw(*m_gameOverText); }
void Game::playingDraw(sf::Time dt) {
    // 1. ���������ڵף�
    m_window.clear(sf::Color::Black);

    // 2. �����ģ���ɫ��һ�ۿɼ���
    if (m_leftPaddle)   m_window.draw(*m_leftPaddle);
    if (m_rightPaddle)  m_window.draw(*m_rightPaddle);

	// 3. ����
    if (m_ball)         m_window.draw(*m_ball);

    // 4. �������
    if (m_fpsText)   m_window.draw(*m_fpsText);
    if (m_ballText)  m_window.draw(*m_ballText);
    // 5. ������ʾ
    if (m_helpText) m_window.draw(*m_helpText);

}


// һ������
void Game::reset()
{
    // 1. ����е� + ��ʼ�ٶ�
    if (m_ball)
    {
        m_ball->setPosition(sf::Vector2f(WINDOW_W / 2.f, WINDOW_H / 2.f));
        m_ball->velocity = sf::Vector2f(400.f, 0.f);   // ���ҷ���
        m_ball->collideCooldown = 0;
    }

    // 2. ���ӻ��е�
    if (m_leftPaddle)
        m_leftPaddle->setPosition(sf::Vector2f(20.f, WINDOW_H / 2.f - 50.f));
    if (m_rightPaddle)
        m_rightPaddle->setPosition(sf::Vector2f(WINDOW_W - 35.f, WINDOW_H / 2.f - 50.f));

    // 3. �÷����㣨�����ӣ�
    // m_leftScore = 0;
    // m_rightScore = 0;

    // 4. �����лز˵����õ������Լ�����״̬
    // setState(State::Menu);   // �� ע�͵�
}

