#include "game.h"
#include "ui_game.h"
#include <string>
#include <QString>
#include <sstream>
#include <iomanip>
#include <QApplication>

Game::Game(int _rows, int _cols, QMainWindow* _parentWindow, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Game),
    parentWindow(_parentWindow),
    rows(_rows),
    cols(_cols),
    firstClick(false)
{
    ui->setupUi(this);
    this->setWindowTitle("Minesweeper");
    this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
    this->setWindowIcon(QIcon(":/icons/icons/flag.ico"));

    gameProgress = ui->progressBar;
    gameLayout = ui->gameGrid;


    gameLayout->setSpacing(0);
    gameLayout->setContentsMargins(0, 0, 0, 0);
    gameLayout->setSizeConstraint(QLayout::SetFixedSize);

    this->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &Game::updateTimer);

    maxMines = (int)((rows * cols) / 5);
    Tile::maxFlags = maxMines;
    //qDebug() << QString::number(maxMines);
    Tile::safeTiles = (rows * cols) - maxMines;
    m_safeTiles = (rows * cols) - maxMines;

    gameProgress->setMinimum(0);
    gameProgress->setMaximum(m_safeTiles);
    createBoard();
}

void Game::createBoard()
{
    //qDebug() << "Creating board!\n";
    board = QList<QList<Tile*>>();
    Tile::firstClick = false;
    Tile::safeTiles = (int)rows * cols - maxMines;
    m_safeTiles = rows * cols - maxMines;
    gameProgress->setStyleSheet("QProgressBar::chunk {background-color: #3daee9;}");
    Tile::maxFlags = maxMines;
    Tile::mineTiles = maxMines;
    Tile::gameOver = false;
    Tile::progress = 0;
    gameTime = 0.00f;
    updateTimer();
    for (int r = 0; r < rows; ++r) {
        QList<Tile*> temp{};
        for (int c = 0; c < cols; ++c) {
            temp.append(new Tile({r, c}, this));
            gameLayout->addWidget(temp[c], r, c);

            // Set signals
            connect(temp[c], &Tile::firstTile, this, &Game::placeMines);
            connect(temp[c], &Tile::resetGame, this, &Game::resetGame);
            connect(temp[c], &Tile::flagPlaced, this, &Game::updateFlags);
            connect(temp[c], &Tile::endGame, this, &Game::endGame);
            // Set progressBar update signal
            connect(temp[c], &Tile::tileOpened, this, &Game::updateProgress, Qt::DirectConnection);
        }
        board.append(temp);
    }
    addNeighbours();
    updateProgress();
    updateFlags();
    this->resize(0, 0);
    emit boardCreated();
}

void Game::resetGame()
{
    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < cols; ++c)
        {
            board[r][c]->resetTile();
//            disconnect(board[r][c]);
//            delete board[r][c];
//            gameLayout->removeWidget(board[r][c]);
        }
    }
    gameTimer->stop();
    gameTime = 0.00f;
    Tile::firstClick = false;
    Tile::safeTiles = m_safeTiles;
    Tile::mineTiles = maxMines;
    Tile::maxFlags = maxMines;
    Tile::gameOver = false;
    Tile::progress = 0;
    gameProgress->setStyleSheet("QProgressBar::chunk {background-color: #3daee9;}");
    updateTimer();
    updateFlags();
    updateProgress();
//    createBoard();
}

void Game::placeMines(Tile* firstClicked)
{
    srand(time(nullptr));
    int mines = 0;
    QList<Tile*> safeTiles = firstClicked->getNeighbours();
    safeTiles.append(firstClicked);
    while (mines < maxMines)
    {
        int _row = rand() % rows;
        int _col = rand() % cols;
        Tile* target = board[_row][_col];
        if (std::find(safeTiles.begin(), safeTiles.end(), target) == safeTiles.end() && !target->isMine)
        {
            board[_row][_col]->placeMine();
            ++mines;
        }
    }
    //qDebug() << "Placed " << mines << " mines";
    //qDebug() << "There are " << Tile::safeTiles << " safe tiles!";
    updateFlags();
    numerizeTiles();


    gameTimer->start(10);

    emit minesPlaced();
}

void Game::updateTimer()
{
    std::stringstream s;
    s << std::fixed << std::setprecision(2) << gameTime;
    QString ss = QString::fromStdString(s.str());
    ui->timerNumber->display(ss);
    ui->timerNumber->setDigitCount(QString("%1").arg(ss).length());
    gameTime += 0.01f;
//    qDebug() << QString::fromStdString(ss);
}

void Game::updateProgress()
{
    progress = Tile::progress;
    gameProgress->setValue(progress);
    gameProgress->update();
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    qDebug() << "Current progress: " << progress << "/" << gameProgress->maximum();
}

void Game::addNeighbours()
{
    //qDebug() << "Adding neighbours";
    if (board[0][0]->getNeighbours().size() > 0)
    {
        return;
    }
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            auto tile = board[r][c];
            for (int x = std::max(0, c - 1); x < std::min(cols, c + 2); ++x) {
                for (int y = std::max(0, r - 1); y < std::min(rows, r + 2); ++y)
                {
                    if (tile == board[y][x]) {
                        continue;
                    }
                    tile->addNeighbour(board[y][x]);

                }
            }
        }
    }
    emit neighboursSet();
}

void Game::numerizeTiles()
{
    //qDebug() << "Numerizing tiles";
    for (const auto &list : board) {
        for (auto tile : list){
            tile->setNumber();
        }
    }
    emit tilesNumerized();
}

void Game::updateFlags()
{
    //qDebug() << "Flags updated!";
    ui->flagsNumber->setDigitCount(QString("%1").arg(Tile::maxFlags).length());
    ui->flagsNumber->display(Tile::maxFlags);
    //qDebug() << QString("%1.length() --> ").arg(Tile::maxFlags) << std::to_string(Tile::maxFlags).length();
}

void Game::endGame(bool wonGame)
{
    //qDebug() << "endGame() called";
    gameTimer->stop();
    Tile::gameOver = true;
    if (wonGame)
    {
        gameProgress->setValue(gameProgress->maximum());
        gameProgress->setStyleSheet(QString("QProgressBar::chunk {background-color: #3eff17;}"));
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                auto tile = board[r][c];
                if (tile->isMine)
                {
                    tile->setIcon(QIcon(":/icons/icons/check_icon_30x30.png"));
                    tile->removeText();
                }
                else
                {
                    tile->isRevealed = true;
                    tile->setText();
                }
            }
        }
    }
    else {
        gameProgress->setStyleSheet(QString("QProgressBar::chunk {background-color: #ae3030;}"));
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                auto tile = board[r][c];
                if (tile->isMine)
                {
                    if (!tile->isFlag && !tile->isMine)
                    {
                        tile->setText();
                    }
                    else if (tile->isFlag)
                    {
                        tile->setIcon(QIcon(":/icons/icons/check_icon_30x30.png"));
                        tile->removeText();
                    }
                    else if (tile->isMine)
                    {
                        tile->setIcon(QIcon(":/icons/icons/wrong_check_icon_30x30.png"));
                        tile->removeText();
                    }
                }
                if (tile->isFlag && !tile->isMine)
                {
                    tile->setIcon(QIcon(":/icons/icons/wrong_check_icon_30x30.png"));
                    tile->removeText();
                }
            }
        }
    }
}

void Game::closeEvent(QCloseEvent *event)
{
    //qDebug() << "Closing gameWindow";
    if (gameTimer->isActive())
    {
        //qDebug() << "gameTimer is active during closeEvent";
        gameTimer->stop();
    }
    parentWindow->show();
    event->accept();
    delete this;
}

Game::~Game()
{
    qDebug() << "~Game() called!";
    delete ui;
}
