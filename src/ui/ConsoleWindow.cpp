#include "ConsoleWindow.h"

#include <QVBoxLayout>
#include <QTimer>

#include <QString>

#include "pages/HomePage.h"
#include "pages/EmulatorPage.h"
#include "emulator/EmulatorManager.h"
#include "screens/GameScreen.h"

#include "audio/AudioManager.h"

ConsoleWindow::ConsoleWindow(const std::vector<Game> &games, QWidget *parent) 
    : QWidget(parent)
    , m_games(games)
    , m_gameScreen(new GameScreen(this))
{
    setStyleSheet(QStringLiteral("background-color: #000000;"));

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(m_gameScreen, 1);

    m_gameScreen->loadPage(m_currentPage, m_games);

    if (m_currentPage == Page::SplashScreen) {
        AudioManager::instance().playSplashScreen();
        QTimer::singleShot(4000, this, [this]() {
            m_currentPage = Page::Home;

            m_gameScreen->loadPage(m_currentPage, m_games);

            if (auto *homePage = m_gameScreen->findChild<HomePage *>()) {
                connect(homePage, &HomePage::selectedGameChanged, m_gameScreen, &GameScreen::setGameTitle);
                connect(homePage, &HomePage::launchGame, this, &ConsoleWindow::launchGame);

                if (!m_games.empty())
                {
                    m_gameScreen->setGameTitle(QString::fromStdString(m_games.front().title));
                }
            }            
            m_gameScreen->focusCurrentPage();        });
    }
}

void ConsoleWindow::launchGame(const Game &game)
{
    m_currentPage = Page::Emulator;
    m_currentGame = const_cast<Game *>(&game);

    if (m_emulatorPage) {
        m_emulatorPage->hide();
        m_emulatorPage->deleteLater();
        m_emulatorPage = nullptr;
    }

    m_emulatorPage = new EmulatorPage(game, this);
    connect(m_emulatorPage, &EmulatorPage::backToHome, this, &ConsoleWindow::backToHome);

    m_gameScreen->hide();
    m_emulatorPage->setGeometry(rect());
    m_emulatorPage->show();
    m_emulatorPage->setFocus();
}

void ConsoleWindow::backToHome()
{
    m_currentPage = Page::Home;
    m_currentGame = nullptr;

    if (m_emulatorPage) {
        m_emulatorPage->hide();
        EmulatorManager::instance().stop();
        m_emulatorPage->deleteLater();
        m_emulatorPage = nullptr;
    }

    m_gameScreen->show();
    m_gameScreen->focusCurrentPage();
}
