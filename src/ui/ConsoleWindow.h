#pragma once

#include <vector>

#include <QWidget>

#include "library/Game.h"
#include "ui/pages/pages.hpp"

class GameScreen;
class EmulatorPage;

class ConsoleWindow : public QWidget
{
public:
    explicit ConsoleWindow(const std::vector<Game> &games, QWidget *parent = nullptr);

private:
    std::vector<Game> m_games;
    GameScreen *m_gameScreen;
    EmulatorPage *m_emulatorPage = nullptr;

    Page m_currentPage = Page::SplashScreen;
    Game *m_currentGame = nullptr;

    void launchGame(const Game &game);
    void backToHome();
};
