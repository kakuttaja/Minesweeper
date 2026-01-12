#ifndef TILE_H
#define TILE_H

#include <QDebug>
#include <QMouseEvent>
#include <QObject>
#include <QList>
#include <QPushButton>

struct Position
{
    int row;
    int col;
};

class Tile : public QPushButton
{
    Q_OBJECT
public:
    Tile(Position _pos, QWidget* parent);
    ~Tile();
    static bool firstClick;
    static int maxFlags;
    static int safeTiles;
    static int mineTiles;
    static int progress;
    static bool gameOver;
    void placeMine();
    void addNeighbour(Tile* tile);
    void setNumber();
    void mousePressEvent(QMouseEvent* event);
    bool isMine;
    bool isFlag;
    bool isRevealed;
    void setText();
    void revealTile();
    void removeText();
    void resetTile();
    QList<Tile*> getNeighbours();

signals:
    void tileOpened();
    void floodFill(Tile* tile);
    void firstTile(Tile* pos);
    void flagPlaced();
    void endGame(bool wonGame);
    void resetGame();

private:
    QList<Tile*> neighbours;
    Position pos;
    int threats;

    void floodFiller(Tile* tile);
    void setFlag();
};

#endif // TILE_H
