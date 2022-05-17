#ifndef GAME_H
#define GAME_H

#include <QMainWindow>
#include <QWidget>
#include <QTimer>
#include <QList>
#include <QGridLayout>
#include <QCloseEvent>
#include <QProgressBar>
#include "tile.h"
#include <QFile>

namespace Ui {
class Game;
}

class Game : public QWidget
{
    Q_OBJECT

public:
    explicit Game(int _rows, int _cols, QMainWindow* _parentWindow, QWidget *parent = nullptr);
    ~Game();

signals:
    void boardCreated();
    void minesPlaced();
    void neighboursSet();
    void tilesNumerized();

public slots:
    void endGame(bool wonGame);
    void resetGame();
    void updateFlags();
    void updateTimer();
    void updateProgress();

private:
    Ui::Game *ui;
    QMainWindow* parentWindow;
    int rows;
    int cols;
    int maxMines;
    bool firstClick;
    float gameTime;

    int m_safeTiles;
    int progress;
    Position firstPos;
    QList<QList<Tile*>> board;
    QGridLayout* gameLayout;

    QProgressBar* gameProgress;

    QTimer* gameTimer;

    void closeEvent(QCloseEvent* event) override;

    void createBoard();
    void placeMines(Tile* firstClicked);
    void addNeighbours();
    void numerizeTiles();
};

#endif // GAME_H
