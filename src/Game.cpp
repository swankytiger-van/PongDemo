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


// 常量先硬编码，后续放 Constants.h
constexpr unsigned WINDOW_W = 800;
constexpr unsigned WINDOW_H = 600;
constexpr float    DT = 1.f / 60.f; // 固定 60 逻辑帧


// 居中辅助
static void centreText(sf::Text& text, float x, float y) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(sf::Vector2f(bounds.position.x + bounds.size.x * 0.5f,
        bounds.position.y + bounds.size.y * 0.5f));
    text.setPosition(sf::Vector2f(x, y));
}


//优化碰撞工具
namespace {
    // 线段 (p0→p1) 与“膨胀后的 AABB”相交测试
    // rect  : 原始 AABB
    // radius: 球半径，用于膨胀
    // t     : 输出碰撞比率 [0,1]
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

    // 构造加载（SFML 3 唯一入口）
    m_font.emplace();                       // 先默认构造
    if (!m_font->openFromFile("assets/font.ttf"))  // 再加载
        throw std::runtime_error("Failed to open assets/font.ttf");

    // 原位构造 Text（零拷贝）
    const sf::Font& font = *m_font;
    m_menuText.emplace( font,"PongCore\nSpace: Start  Esc: Exit", 24u);
    m_pauseText.emplace( font,"PAUSED\nSpace: Resume  R: Reset", 24u);
    m_gameOverText.emplace( font,"GAME OVER\nR: Restart  Esc: Exit", 24u);

    // 构造双拍
    m_leftPaddle.emplace(20.f, 250.f, 15.f, 100.f, true);   // 左拍
    m_rightPaddle.emplace(765.f, 250.f, 15.f, 100.f, false); // 右拍


    m_fpsText.emplace(*m_font, "FPS: --", 20u);
    m_fpsText->setFillColor(sf::Color::White);
    m_fpsText->setPosition(sf::Vector2f(10.f, 10.f));
    

    m_ballText.emplace(*m_font, "Ball pos: --", 20u);
    m_ballText->setFillColor(sf::Color::White);
    m_ballText->setPosition(sf::Vector2f(10.f, 35.f));

	// 构造球
    m_ball.emplace(400.f, 300.f, 10.f);
    m_ball->velocity = sf::Vector2f(400.f, 0.f);

    // 绑定出界回调
    m_ball->setOutOfBoundsCallback([this](bool isLeft) {
        if (isLeft)  
            leftScore();   // 接口
        else         
            rightScore();  // 接口

		
        });
    //样式 & 位置
    m_menuText->setFillColor(sf::Color::White);
    centreText(*m_menuText, WINDOW_W / 2.f, WINDOW_H / 2.f - 40.f);

    m_pauseText->setFillColor(sf::Color::Yellow);
    centreText(*m_pauseText, WINDOW_W / 2.f, WINDOW_H / 2.f);

    m_gameOverText->setFillColor(sf::Color::Red);
    centreText(*m_gameOverText, WINDOW_W / 2.f, WINDOW_H / 2.f);

    // 菜单卡片
    m_menuCard.emplace();                       // ① 默认构造
    m_menuCard->init(sf::Vector2f(400, 300), sf::Color(0, 0, 0, 220)); // ② 事后初始化
    m_menuCard->setPosition(sf::Vector2f(WINDOW_W / 2.f - 200.f, WINDOW_H / 2.f - 150.f));


    // 大标题
    m_titleText.emplace(*m_font, "PONG  CORE", 60u);   // 三个参数都要
    m_titleText->setFillColor(sf::Color(0, 255, 150));
    centreText(*m_titleText, WINDOW_W / 2.f, WINDOW_H / 2.f - 120.f);
    
    // 开始/退出 按钮
    auto addBtn = [&](const std::string& label, float y) {
        m_menuBtns.emplace_back();                       // 推空 optional
        auto& btn = m_menuBtns.back().emplace();         // 默认构造

        btn.rect.setSize(sf::Vector2f(200, 50));
        btn.rect.setFillColor(sf::Color(50, 50, 50));
        btn.rect.setOutlineThickness(2);
        btn.rect.setOutlineColor(sf::Color::White);
        btn.rect.setPosition(sf::Vector2f(WINDOW_W / 2.f - 100.f, y));

        btn.init(*m_font, label, 24u);                   // 填真实数据

        if (btn.text)    // 确保已构造
            centreText(*btn.text,
                btn.rect.getPosition().x + 100.f,
                btn.rect.getPosition().y + 25.f);
        };
    addBtn("START", WINDOW_H / 2.f - 40.f);
    addBtn("EXIT", WINDOW_H / 2.f + 40.f);

    m_helpText.emplace(*m_font, "Left: W/S   Right: UP/DN   ESC: Menu   Space:Pause", 18u);
    m_helpText->setFillColor(sf::Color(200, 200, 200, 180));
    centreText(*m_helpText, WINDOW_W / 2.f, WINDOW_H - 20.f);

    // 得分文字
    m_leftScoreText.emplace(*m_font, "0", 48u);
    m_leftScoreText->setFillColor(sf::Color::Green);
    m_rightScoreText.emplace(*m_font, "0", 48u);
    m_rightScoreText->setFillColor(sf::Color::Green);
    // 顶部居中
    centreText(*m_leftScoreText, WINDOW_W / 2.f - 60.f, 40.f);
    centreText(*m_rightScoreText, WINDOW_W / 2.f + 60.f, 40.f);

	// 倒计时文字
    // 倒计时
    m_timeText.emplace(*m_font, "60", 36u);
    m_timeText->setFillColor(sf::Color::Yellow);
    centreText(*m_timeText, WINDOW_W / 2.f, 35.f);

    // 发球倒计时
    m_serveText.emplace(*m_font, "3", 120u);
    m_serveText->setFillColor(sf::Color(255, 255, 0, 220)); // 黄
    centreText(*m_serveText, WINDOW_W / 2.f, WINDOW_H / 2.f - 60.f);
    m_serving = true;
    m_serveTimer = 0.f;
}

void Game::run()
{
    sf::Clock clock;
    while (m_window.isOpen())
    {
        sf::Time dt = clock.restart();
        if (dt.asSeconds() > 0.05f) dt = sf::seconds(0.05f);

        // 1. 事件
        while (const auto optEv = m_window.pollEvent())
        {
            handleEvent(*optEv);
        }

        // 2. 逻辑
        update(dt);

        // 3. 渲染
        m_window.clear(sf::Color::Black);
        draw(dt);
        m_window.display();
    }
}


// 事件分发
void Game::handleEvent(const sf::Event& ev)
{
    // 1. 菜单按钮点击（只在菜单状态处理）
    if (m_state == State::Menu && ev.is<sf::Event::MouseButtonPressed>())
    {
        auto mp = sf::Vector2f(ev.getIf<sf::Event::MouseButtonPressed>()->position);
        for (auto& opt : m_menuBtns)
        {
            if (!opt) continue;
            Button& btn = *opt;
            if (btn.click(mp))
            {
                // 点击下沉动画
                btn.rect.setScale(sf::Vector2f(0.95f, 0.95f));

                if (btn.text->getString() == "START")
                {
                    setState(State::Playing);
                    reset();          // 新开一局
                }
                else if (btn.text->getString() == "EXIT")
                {
                    m_window.close();
                }
                break; // 只点一个
            }
        }
        return; // 菜单事件处理完就退，不再走键盘
    }

    // 2. 原键盘逻辑
    if (auto kp = ev.getIf<sf::Event::KeyPressed>())
    {
        sf::Keyboard::Key key = kp->code;
        if (key == sf::Keyboard::Key::Escape)
        {
            // 新逻辑：菜单→退出，其他→回菜单
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
// 更新分发
void Game::update(sf::Time dt) {

    constexpr float winW = 800.f;
    constexpr float winH = 600.f;
    float x = 0.f, r = 0.f;static sf::Clock timer;
    switch (m_state) {
    case State::Menu:    
        menuUpdate(dt);    

        break;
    case State::Playing:
        
        // 0. 发球倒计时（只在 Playing 且 serving）
        if (m_state == State::Playing && m_serving)
        {
            m_serveTimer += dt.asSeconds();
            if (m_serveTimer >= 1.f)
            {
                m_serveTimer = 0.f;
                --m_serveCount;
                if (m_serveText) m_serveText->setString(std::to_string(m_serveCount));
                // 倒计时结束，立刻关闭发球阶段
                if (m_serveCount <= 0)
                {
                    m_serving = false;                // 1. 关闭标志
					m_ball->serveImmunity = 0; // 解除免疫
                    if (m_serveText) m_serveText->setString(""); // 2. 隐藏数字
                    if (m_ball)
                    {
                        // 4. 正式给速度（方向随机可再加）
                        m_ball->velocity = sf::Vector2f(400.f, 0.f);
                    }
                }
            }
            
            return; // 倒计时期间不更新球、拍子
        }
        // 1. 更新拍子
        if (m_leftPaddle)   m_leftPaddle->update(dt.asSeconds(), true);
        if (m_rightPaddle)  m_rightPaddle->update(dt.asSeconds(), false);

        // 2. 更新球
        if (m_ball) {
            if (m_ball->collideCooldown > 0) --m_ball->collideCooldown;
            m_ball->update(dt.asSeconds());
        }      
        playingDraw(dt);

		
        // 3. 碰撞检测（放在球更新之后，防止“穿模”,）
        if (m_ball && m_leftPaddle && m_ball->collideCooldown == 0 && m_ball->serveImmunity == 0) {
            sf::FloatRect paddleAABB = m_leftPaddle->getGlobalBounds();
            float t = 1.f;
            if (sweepSegmentAABB(m_ball->prevPos,
                m_ball->getPosition(),
                paddleAABB,
                m_ball->getRadius(),
                t))
            {
                // 1. 回滚到碰撞点
                sf::Vector2f hitPos = m_ball->prevPos +
                    t * (m_ball->getPosition() - m_ball->prevPos);
                m_ball->setPosition(hitPos);

                // 2. 根据拍面法线把球“推”到拍子外侧
                float paddleRight = paddleAABB.position.x + paddleAABB.size.x;
                float ballR = m_ball->getRadius();
                m_ball->setPosition(sf::Vector2f(paddleRight + ballR,
                    m_ball->getPosition().y));

                // 3. 反弹
                m_ball->hitPaddle(m_leftPaddle->centerY(),
                    m_leftPaddle->halfHeight());

                // 4. 加冷却（帧数随意，2~3 帧即可）
                m_ball->collideCooldown = 3;
                std::cout << "[Game] left paddle swept hit\n";
            }
        }

        // 右侧拍完全对称
        if (m_ball && m_rightPaddle && m_ball->collideCooldown == 0 && m_ball->serveImmunity == 0) {
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

        
		//倒计时 
        
        if (timer.getElapsedTime() >= sf::seconds(1.f))
        {
            timer.restart();
            if (m_countDown > 0)
            {
                --m_countDown;
                if (m_timeText) m_timeText->setString(std::to_string(m_countDown));
            }
            else
            {
                // 时间到 → 进入结束页
                setState(State::GameOver);
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
    // 更新调试面板
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
    // 淡入淡出
    if (m_state != State::Playing)
        m_fadeAlpha = std::min(m_fadeAlpha + 5, 200);
    else
        m_fadeAlpha = std::max(m_fadeAlpha - 5, 0);
}

// 绘制分发
void Game::draw(sf::Time dt){
    m_window.clear(sf::Color::Black);   // 统一清屏
    sf::RectangleShape fade;
    fade.setSize(sf::Vector2f(WINDOW_W, WINDOW_H));
    fade.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(m_fadeAlpha)));
    m_window.draw(fade);
    switch (m_state) {
    case State::Menu:
        menuDraw();
        break;
    case State::Playing:
        playingDraw(dt);   // 后续画球拍 & 球
        break;
    case State::Paused:
        playingDraw(dt);   // 继续画场景，只是叠一层文字
        pausedDraw(dt);
        break;
    case State::GameOver:
        playingDraw(dt);   // 保留场地
        gameOverDraw();
        break;
    }

    m_window.display();   // 统一送显
}

// 各状态空实现，后续填逻辑
void Game::menuUpdate(sf::Time dt)
{
    // 呼吸标题（可选）
    static float breath = 0.f;
    breath += dt.asSeconds() * 3.f;
    float scale = 1.f + std::sin(breath) * 0.05f;
    if (m_titleText)
        m_titleText->setScale(sf::Vector2f(scale, scale));

    // 悬停检测
    sf::Vector2f mp = sf::Vector2f(sf::Mouse::getPosition(m_window));
    for (auto& opt : m_menuBtns)
    {
        if (!opt) continue;
        Button& btn = *opt;
        bool nowHover = btn.click(mp);
        if (nowHover && !btn.hovered)      // 悬停进入
        {
            btn.rect.setFillColor(sf::Color(80, 80, 80));
            btn.rect.setScale(sf::Vector2f(1.05f, 1.05f));
        }
        else if (!nowHover && btn.hovered) // 悬停离开
        {
            btn.rect.setFillColor(sf::Color(50, 50, 50));
            btn.rect.setScale(sf::Vector2f(1.f, 1.f));
        }
        btn.hovered = nowHover;
        if (!btn.hovered) btn.rect.setScale(sf::Vector2f(1.f, 1.f)); // 松手弹回
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
    // 1. 卡片背景
    m_window.draw(*m_menuCard);
    // 2. 标题
    m_window.draw(*m_titleText);
    // 3. 按钮
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
void Game::gameOverDraw() { 
    // 胜者大文字
    std::string winner = (m_leftScore == 11 || m_countDown == 0 && m_leftScore > m_rightScore) ?
        "LEFT WINS!" : "RIGHT WINS!";
    sf::Text winText(*m_font, winner, 60u);
    winText.setFillColor(sf::Color::Yellow);
    centreText(winText, WINDOW_W / 2.f, WINDOW_H / 2.f - 60.f);
    m_window.draw(winText);
}
void Game::playingDraw(sf::Time dt) {
    // 1. 画场景（黑底）
    m_window.clear(sf::Color::Black);

    // 2. 画球拍（红色，一眼可见）
    if (m_leftPaddle)   m_window.draw(*m_leftPaddle);
    if (m_rightPaddle)  m_window.draw(*m_rightPaddle);

	// 3. 画球
    if (m_ball)         m_window.draw(*m_ball);

    // 4. 调试面板
    if (m_fpsText)   m_window.draw(*m_fpsText);
    if (m_ballText)  m_window.draw(*m_ballText);
    // 5. 底部提示
    if (m_helpText) m_window.draw(*m_helpText);

    // 6. 比分 + 倒计时
    if (m_leftScoreText)  m_window.draw(*m_leftScoreText);
    if (m_rightScoreText) m_window.draw(*m_rightScoreText);
    if (m_timeText)       m_window.draw(*m_timeText);

    if (m_serving && m_serveText)
    {
        m_window.draw(*m_serveText);
    }

}

// 得分接口
void Game::leftScore()
{
    ++m_leftScore;
    if (m_leftScoreText)                 // 必须判空
    {
        m_leftScoreText->setString(std::to_string(m_leftScore)); // ← 实时更新
        std::cout << "[Score] left = " << m_leftScore << "\n";
    }
        
    scoreBlink();
    if (m_leftScore == 11 || m_countDown == 0)
    {
        // 胜者出现 → 全局清零
        newMatch();
        setState(State::GameOver);
    }
    else reset();
}

void Game::rightScore()
{
    ++m_rightScore;
    if (m_rightScoreText)
        m_rightScoreText->setString(std::to_string(m_rightScore)); // ← 实时更新

    scoreBlink();
    if (m_rightScore == 11 || m_countDown == 0)
    {
        // 胜者出现 → 全局清零
        newMatch();
        setState(State::GameOver);
    }
    else reset();
}

void Game::newMatch()
{
    m_leftScore = 0;
    m_rightScore = 0;
    m_countDown = 60;
    if (m_leftScoreText)  m_leftScoreText->setString("0");
    if (m_rightScoreText) m_rightScoreText->setString("0");
    if (m_timeText)       m_timeText->setString("60");
}

// 一局重置
void Game::reset()
{
    
    

// 1. 球回中点 + 初始速度
    if (m_ball)
    {
        m_ball->setPosition(sf::Vector2f(WINDOW_W / 2.f, WINDOW_H / 2.f));
        m_ball->velocity = sf::Vector2f(400.f, 0.f);   // 向右发球
        m_ball->collideCooldown = 0;
    }
	// 拍子回初始位置

    
    if (m_leftPaddle)  m_leftPaddle->setPosition(sf::Vector2f(20.f, WINDOW_H / 2.f - 50.f));
    if (m_rightPaddle) m_rightPaddle->setPosition(sf::Vector2f(WINDOW_W - 35.f, WINDOW_H / 2.f - 50.f));


	//m_ball->serveImmunity = 0; 

    // 2. 重置发球倒计时
    m_serving = true;
    m_serveCount = 3;
    m_serveTimer = 0.f;
    if (m_serveText) m_serveText->setString("3");


    

    // 4. 不再切回菜单！让调用者自己决定状态
    // setState(State::Menu);   // ← 注释掉
}

