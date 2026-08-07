#pragma once

#include <QPaintEvent>
#include <QWidget>
#include <vector>

#include "library/Game.h"
#include "ui/pages/pages.hpp"

class QVBoxLayout;

class GameScreen : public QWidget
{
public:
    explicit GameScreen(QWidget *parent = nullptr);

    void loadPage(Page page, const std::vector<Game> &games = {});
    void setGameTitle(const QString &gameTitle);
    void focusCurrentPage();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVBoxLayout *m_layout = nullptr;
    Page m_currentPage = Page::Home;
    QString m_gameTitle;
    QWidget *m_pageWidget = nullptr;
    std::vector<Game> m_games;
};
