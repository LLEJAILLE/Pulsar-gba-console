#include "GameScreen.h"

#include "../pages/HomePage.h"
#include "../pages/SplashScreen.h"
#include "../../utils/env.hpp"

#include <QLabel>
#include <QPainter>
#include <QVBoxLayout>

namespace
{
    const QColor kBackgroundColor("#303030");
    const QColor kBorderColor("#5a5a5a");
    const QColor kTextColor("#f0f0f0");
}

GameScreen::GameScreen(QWidget *parent) : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(SCREEN_SIZE_X, SCREEN_SIZE_Y);

    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
}

void GameScreen::loadPage(Page page, const std::vector<Game> &games)
{
    if (m_currentPage == page && m_pageWidget != nullptr) {
        return;
    }

    m_currentPage = page;
    m_games = games;

    if (m_pageWidget != nullptr) {
        m_layout->removeWidget(m_pageWidget);
        delete m_pageWidget;
        m_pageWidget = nullptr;
    }

    if (page == Page::SplashScreen) {
        m_pageWidget = new SplashScreen(this);
    } else if (page == Page::Home) {
        auto *homePage = new HomePage(m_games, this);
        m_pageWidget = homePage;
        
        if (!m_gameTitle.isEmpty()) {
            homePage->setGameTitle(m_gameTitle);
        }
    } else {
        auto *placeholder = new QLabel(QStringLiteral("Page not implemented yet"), this);
        placeholder->setAlignment(Qt::AlignCenter);
        m_pageWidget = placeholder;
    }

    m_layout->addWidget(m_pageWidget);
}

void GameScreen::setGameTitle(const QString &gameTitle)
{
    if (m_gameTitle == gameTitle) {
        return;
    }

    m_gameTitle = gameTitle;

    if (auto *homePage = qobject_cast<HomePage *>(m_pageWidget)) {
        homePage->setGameTitle(m_gameTitle);
    }

    update();
}

void GameScreen::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.fillRect(rect(), kBackgroundColor);
    painter.setPen(QPen(kBorderColor, 2));
    painter.drawRect(rect().adjusted(1, 1, -2, -2));
}

void GameScreen::focusCurrentPage()
{
    if (m_pageWidget) {
        m_pageWidget->setFocus(Qt::OtherFocusReason);
    }
}
