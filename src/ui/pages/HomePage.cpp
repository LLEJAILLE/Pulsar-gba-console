#include "HomePage.h"

#include "../../audio/AudioManager.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
#include <QKeyEvent>
#include <QTimer>
#include <QResizeEvent>
#include <QHideEvent>
#include <QShowEvent>
#include <QFile>
#include <QApplication>

#include <ctime>
#include <filesystem>

HomePage::HomePage(const std::vector<Game> &games, QWidget *parent)
    : QWidget(parent)
    , m_scrollArea(nullptr)
    , m_container(nullptr)
    , m_gamesLayout(nullptr)
    , m_gamesList(nullptr)
{
    AudioManager::instance().setVolume(0.2f);

    setObjectName(QStringLiteral("HomePage"));
    setStyleSheet("background-color: transparent;");
    setFocusPolicy(Qt::StrongFocus);

    m_games = games;
    currentTimeString = currentTime();

    m_background = new BackgroundWidget(this);

    // Main vertical layout: header + content + footer
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 20, 40, 20);
    mainLayout->setSpacing(20);

    // Header layout: title on left, battery on right
    auto *headerLayout = new QHBoxLayout();
    headerLayout->setContentsMargins(0, 0, 0, 0);
    
    auto *titleLabel = new QLabel("Pulsar", this);
    titleLabel->setStyleSheet(
        "color: white;"
        "font-size: 28px;"
        "font-weight: bold;"
    );
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    
    auto *batteryLabel = new QLabel("🔋 100%", this);
    batteryLabel->setStyleSheet(
        "color: #00ff00;"
        "font-size: 14px;"
        "font-weight: bold;"
    );
    headerLayout->addWidget(batteryLabel);
    
    mainLayout->addLayout(headerLayout);

    // Content layout: left panel (for covers) + right panel (for games list)
    auto *contentLayout = new QHBoxLayout();
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(40);

    // Left panel - game cover (25% width)
    auto *leftPanel = new QWidget(this);
    leftPanel->setStyleSheet("background-color: transparent;");
    auto *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(20);
    
    m_titleLabel = new QLabel(this);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setMinimumSize(320, 320);
    m_titleLabel->setMaximumSize(320, 320);
    m_titleLabel->setStyleSheet(
        "border-radius: 16px;"
        "background-color: transparent;"
    );
    leftLayout->addWidget(m_titleLabel, 0, Qt::AlignHCenter);
    leftLayout->addStretch();
    
    contentLayout->addWidget(leftPanel, 0);

    // Right panel - games list (75% width)
    auto *rightPanel = new QWidget(this);
    rightPanel->setStyleSheet("background-color: transparent;");
    auto *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    // Games list with better styling
    m_gamesList = new QListWidget(this);
    m_gamesList->setStyleSheet(
        "QListWidget {"
        "  background-color: transparent;"
        "  border: none;"
        "  outline: none;"
        "  padding: 0px;"
        "}"
        "QListWidget::item {"
        "  padding: 16px 24px;"
        "  margin: 8px 0px;"
        "  border-radius: 12px;"
        "  background-color: rgba(50, 50, 50, 200);"
        "  backdrop-filter: blur(10px);"
        "  border: 2px solid rgba(60, 60, 60, 150);"
        "  color: white;"
        "  font-size: 16px;"
        "  font-weight: 500;"
        "  min-height: 30px;"
        "}"
        "QListWidget::item:hover {"
        "  background-color: rgba(20, 35, 65, 220);"
        "  border: 2px solid rgba(0, 200, 255, 100);"
        "}"
        "QListWidget::item:selected {"
        "  background-color: rgba(0, 120, 180, 140);"
        "  border: 3px solid #40e0ff;"
        "  color: white;"
        "  padding: 14px 22px;"
        "}"
        "QScrollBar:vertical {"
        "  background-color: transparent;"
        "  width: 10px;"
        "}"
        "QScrollBar::handle:vertical {"
        "  background-color: #00c8ff;"
        "  border-radius: 5px;"
        "  min-height: 20px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "  background-color: #00d4ff;"
        "}"
    );
    m_gamesList->setFocusPolicy(Qt::NoFocus);
    m_gamesList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_gamesList->setWordWrap(true);
    m_gamesList->setSpacing(2);

    // Populate games list
    for (const auto &game : games) {
        auto *item = new QListWidgetItem(QString::fromStdString(game.title), m_gamesList);
        item->setFont(QFont("Arial", 14, QFont::Medium));
        item->setSizeHint(QSize(0, 70));
        m_gameTitles.push_back(QString::fromStdString(game.title));
    }

    rightLayout->addWidget(m_gamesList);
    contentLayout->addWidget(rightPanel, 1);

    mainLayout->addLayout(contentLayout, 1);

    // Footer layout: controls
    auto *footerLayout = new QHBoxLayout();
    footerLayout->setContentsMargins(0, 0, 0, 0);
    footerLayout->setSpacing(30);
    
    auto *playLabel = new QLabel("⭕ Play", this);
    playLabel->setStyleSheet(
        "color: #00c8ff;"
        "font-size: 14px;"
    );
    footerLayout->addWidget(playLabel);
    
    auto *backLabel = new QLabel("⭕ Back", this);
    backLabel->setStyleSheet(
        "color: #ff6b6b;"
        "font-size: 14px;"
    );
    footerLayout->addWidget(backLabel);
    
    auto *selectLabel = new QLabel("⊕ Select", this);
    selectLabel->setStyleSheet(
        "color: #888;"
        "font-size: 14px;"
    );
    footerLayout->addWidget(selectLabel);
    
    auto *optionsLabel = new QLabel("⋮ Options", this);
    optionsLabel->setStyleSheet(
        "color: #888;"
        "font-size: 14px;"
    );
    footerLayout->addWidget(optionsLabel);
    
    footerLayout->addStretch();
    
    mainLayout->addLayout(footerLayout);

    m_timeTimer = new QTimer(this);
    connect(m_timeTimer, &QTimer::timeout, this, &HomePage::updateCurrentTime);
    m_timeTimer->start(1000);

    if (!m_games.empty()) {
        setCurrentIndex(0);
        m_gamesList->setCurrentRow(0);
        updateCover();
    }
}

void HomePage::setGameTitle(const QString &gameTitle)
{
    Q_UNUSED(gameTitle);
}

void HomePage::updateTileSizes()
{
}

void HomePage::updateSelection()
{
}

void HomePage::centerCurrentTile()
{
}

void HomePage::setCurrentIndex(int newIndex)
{
    if (m_games.empty()) {
        return;
    }

    // Circular navigation
    int size = static_cast<int>(m_games.size());
    m_currentIndex = ((newIndex % size) + size) % size;
}

void HomePage::updateCover()
{
    if (m_currentIndex < 0 || m_currentIndex >= static_cast<int>(m_games.size())) {
        return;
    }

    const Game &game = m_games[m_currentIndex];
    QPixmap pixmap;

    // Get the game name without extension
    std::string gameName = game.romPath.stem().string();
    
    // Try different search paths for the icon
    std::vector<std::filesystem::path> searchPaths;
    
    // 1. Relative to current working directory
    searchPaths.push_back(std::filesystem::path("assets") / "icons");
    
    // 2. Relative to application directory
    QString appDir = QApplication::applicationDirPath();
    searchPaths.push_back(std::filesystem::path(appDir.toStdString()) / ".." / "assets" / "icons");
    searchPaths.push_back(std::filesystem::path(appDir.toStdString()) / "assets" / "icons");
    
    // Try to find cover image in each search path
    for (const auto &iconDir : searchPaths) {
        for (const auto &ext : {".png", ".jpg", ".jpeg", ".PNG", ".JPG", ".JPEG"}) {
            std::filesystem::path iconPath = iconDir / (gameName + ext);
            if (std::filesystem::exists(iconPath)) {
                pixmap.load(QString::fromStdString(iconPath.string()));
                if (!pixmap.isNull()) {
                    break;
                }
            }
        }
        if (!pixmap.isNull()) {
            break;
        }
    }

    // If not found, try game directory
    if (pixmap.isNull()) {
        std::filesystem::path gameDir = game.romPath.parent_path();
        
        for (const auto &ext : {".png", ".jpg", ".jpeg"}) {
            std::filesystem::path coverPath = gameDir / (gameName + ext);
            if (std::filesystem::exists(coverPath)) {
                pixmap.load(QString::fromStdString(coverPath.string()));
                if (!pixmap.isNull()) {
                    break;
                }
            }
        }
    }

    // If no cover found, create a placeholder
    if (pixmap.isNull()) {
        pixmap = QPixmap(320, 320);
        pixmap.fill(QColor(26, 31, 58));
        
        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QColor(100, 120, 150));
        painter.setFont(QFont("Arial", 14, QFont::Bold));
        painter.drawText(pixmap.rect(), Qt::AlignCenter | Qt::TextWordWrap, 
                        QString::fromStdString(game.title));
    }

    // Scale and apply rounded corners to the cover image
    const int coverSize = 280;
    QPixmap scaled = pixmap.scaled(QSize(coverSize, coverSize), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QPixmap roundedPixmap(coverSize, coverSize);
    roundedPixmap.fill(Qt::transparent);

    QPainter painter(&roundedPixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addRoundedRect(QRectF(0, 0, coverSize, coverSize), 24.0, 24.0);
    painter.setClipPath(path);

    const int x = (coverSize - scaled.width()) / 2;
    const int y = (coverSize - scaled.height()) / 2;
    painter.drawPixmap(x, y, scaled);
    painter.end();

    m_titleLabel->setPixmap(roundedPixmap);
}

void HomePage::resizeEvent(QResizeEvent *event)
{
    if (m_background) {
        m_background->setGeometry(rect());
    }
    QWidget::resizeEvent(event);
}

void HomePage::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    AudioManager::instance().playHomeMusic();
}

void HomePage::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
    AudioManager::instance().stopHomeMusic();
}

void HomePage::keyPressEvent(QKeyEvent *event)
{
    if (m_games.empty()) {
        event->ignore();
        return;
    }

    switch (event->key()) {
    case Qt::Key_Down:
    {
        int newIndex = m_currentIndex + 1;
        setCurrentIndex(newIndex);
        m_gamesList->setCurrentRow(m_currentIndex);
        
        // Scroll to center the item
        m_gamesList->scrollToItem(m_gamesList->item(m_currentIndex), QAbstractItemView::PositionAtCenter);
        
        updateCover();
        AudioManager::instance().playSwitchMenuItem();
        event->accept();
        return;
    }

    case Qt::Key_Up:
    {
        int newIndex = m_currentIndex - 1;
        setCurrentIndex(newIndex);
        m_gamesList->setCurrentRow(m_currentIndex);
        
        // Scroll to center the item
        m_gamesList->scrollToItem(m_gamesList->item(m_currentIndex), QAbstractItemView::PositionAtCenter);
        
        updateCover();
        AudioManager::instance().playSwitchMenuItem();
        event->accept();
        return;
    }

    case Qt::Key_Space:
    case Qt::Key_A:
        if (m_currentIndex >= 0 && m_currentIndex < static_cast<int>(m_games.size())) {
            emit launchGame(m_games[m_currentIndex]);
        }
        event->accept();
        return;

    default:
        QWidget::keyPressEvent(event);
        return;
    }
}

std::string HomePage::currentTime() const
{
    char buffer[20];
    std::time_t now = std::time(nullptr);
    std::tm localTime;
#ifdef _WIN32
    localtime_s(&localTime, &now);
#else
    localtime_r(&now, &localTime);
#endif
    std::strftime(buffer, sizeof(buffer), "%d/%m %H:%M", &localTime);
    return std::string(buffer);
}

void HomePage::updateCurrentTime()
{
    std::string newTimeString = currentTime();
    if (newTimeString != currentTimeString) {
        currentTimeString = newTimeString;
    }
}

void HomePage::updateBatteryLevel()
{
}
