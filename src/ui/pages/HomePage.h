#pragma once
#include "library/Game.h"
#include "../widgets/BackgroundWidget.h"
#include "../widgets/BatteryWidget.h"
#include "../../audio/AudioManager.h"

#include <vector>
#include <QString>
#include <QWidget>
#include <QPropertyAnimation>
#include <QPainter>
#include <filesystem>

class GameTileWidget;
class QLabel;
class QHBoxLayout;
class QKeyEvent;
class QResizeEvent;
class QScrollArea;
class QShowEvent;
class QTimer;
class QListWidget;
class BatteryWidget;

class HomePage : public QWidget
{
    Q_OBJECT

public:
    explicit HomePage(const std::vector<Game> &games, QWidget *parent = nullptr);

    void setGameTitle(const QString &gameTitle);

    void updateCover();

signals:
    void selectedGameChanged(const QString &gameTitle);
    void launchGame(const Game &game);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void updateTileSizes();
    void updateSelection();
    void centerCurrentTile();
    void setCurrentIndex(int newIndex);

    void updateCurrentTime();
    void updateBatteryLevel();

    std::string currentTime() const;

    std::vector<Game> m_games;
    std::vector<QString> m_gameTitles;
    std::vector<GameTileWidget *> m_tiles;
    int m_currentIndex = 0;
    int m_batteryLevel = 100;

    QScrollArea *m_scrollArea = nullptr;
    QWidget *m_container = nullptr;
    QHBoxLayout *m_gamesLayout = nullptr;
    QListWidget *m_gamesList = nullptr;
    BackgroundWidget *m_background = nullptr;
    QWidget *m_overlay = nullptr;
    QLabel *m_titleLabel = nullptr;
    QLabel *m_gameTitleLabel = nullptr;
    QLabel *m_hourLabel = nullptr;
    BatteryWidget *m_batteryWidget = nullptr;
    QPropertyAnimation *m_scrollAnimation = nullptr;
    QTimer *m_timeTimer = nullptr;

    std::string currentTimeString;
};
