#include "Game.h"
#include <SFML/Graphics.hpp>
#include <SFML/Config.hpp>
#include <iostream>
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

Game::Game(unsigned width, unsigned height, const std::string& title)
    : m_window(sf::VideoMode({ width, height }), title)
{
    m_window.setVerticalSyncEnabled(true);

    // 1. 构造加载（SFML 3 唯一入口）
    m_font.emplace();                       // 先默认构造
    if (!m_font->openFromFile("assets/font.ttf"))  // 再加载
        throw std::runtime_error("Failed to open assets/font.ttf");

    // 2. 原位构造 Text（零拷贝）
    const sf::Font& font = *m_font;
    m_menuText.emplace( font,"PongCore\nSpace: Start  Esc: Exit", 24u);
    m_pauseText.emplace( font,"PAUSED\nSpace: Resume  R: Reset", 24u);
    m_gameOverText.emplace( font,"GAME OVER\nR: Restart  Esc: Exit", 24u);


    // 3. 样式 & 位置
    m_menuText->setFillColor(sf::Color::White);
    centreText(*m_menuText, WINDOW_W / 2.f, WINDOW_H / 2.f - 40.f);

    m_pauseText->setFillColor(sf::Color::Yellow);
    centreText(*m_pauseText, WINDOW_W / 2.f, WINDOW_H / 2.f);

    m_gameOverText->setFillColor(sf::Color::Red);
    centreText(*m_gameOverText, WINDOW_W / 2.f, WINDOW_H / 2.f);
}

void Game::run() {
    sf::Clock clock;
    while (m_window.isOpen()) {
        sf::Time dt = clock.restart();
        if (dt.asSeconds() > 0.05f) dt = sf::seconds(0.05f);

        // 用 optional 接住事件
        while (auto ev = m_window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) m_window.close();
            handleEvent(*ev);
        }

        update(dt);
        m_window.clear(sf::Color::Black);
        draw();
        m_window.display();
    }
}

// 事件分发
// 事件分发
void Game::handleEvent(const sf::Event& ev) {
    // 单层解包即可
    if (auto kp = ev.getIf<sf::Event::KeyPressed>()) {
        sf::Keyboard::Key key = kp->code;
        if (key == sf::Keyboard::Key::Escape) {
            m_window.close();
        }
        else if (key == sf::Keyboard::Key::Space) {
            if (m_state == State::Menu || m_state == State::Paused)
                setState(State::Playing);
            else if (m_state == State::Playing)
                setState(State::Paused);
        }
        else if (key == sf::Keyboard::Key::R) {
            reset();
        }
    }
    


}

// 更新分发
void Game::update(sf::Time dt) {
    switch (m_state) {
    case State::Menu:    menuUpdate(dt);    break;
    case State::Playing: playingUpdate(dt); break;
    case State::Paused:  pausedUpdate(dt);  break;
    case State::GameOver:gameOverUpdate(dt); break;
    }
}

// 绘制分发
void Game::draw() {
    m_window.clear(sf::Color::Black);   // 统一清屏

    switch (m_state) {
    case State::Menu:
        menuDraw();
        break;
    case State::Playing:
        playingDraw();   // 后续画球拍 & 球
        break;
    case State::Paused:
        playingDraw();   // 继续画场景，只是叠一层文字
        pausedDraw();
        break;
    case State::GameOver:
        playingDraw();   // 保留场地
        gameOverDraw();
        break;
    }

    m_window.display();   // 统一送显
}

// 各状态空实现，后续填逻辑
void Game::menuUpdate(sf::Time dt) { /*TODO*/ }
void Game::playingUpdate(sf::Time dt) { /*TODO*/ }
void Game::pausedUpdate(sf::Time dt) { /*TODO*/ }
void Game::gameOverUpdate(sf::Time dt) { /*TODO*/ }

void Game::menuDraw() { if (m_menuText)   m_window.draw(*m_menuText); }
void Game::pausedDraw() { if (m_pauseText)  m_window.draw(*m_pauseText); }
void Game::gameOverDraw() { if (m_gameOverText)m_window.draw(*m_gameOverText); }
void Game::playingDraw() { /*TODO 画球拍 & 球*/ }


// 一局重置
void Game::reset() {
    setState(State::Menu);
    // 后续重置球位置、得分
}

