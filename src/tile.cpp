#include "tile.h"

QString buttonStyle =
"QPushButton { "
"background: rgba(255, 255, 255, 20);"
"border-style: outset;"
"border-width: 1px;"
"border-color: rgba(255, 255, 255, 30);"
"padding: 0px;"
"}"
"QPushButton:hover {"
"background: rgba(255, 255, 255, 30);"
"}";

QString flagStyle =
"QPushButton { "
"background: rgba(255, 255, 255, 20);"
"color: crimson;"
"border-style: outset;"
"border-width: 1px;"
"border-color: rgba(255, 255, 255, 30);"
"padding: 0px;"
"font-weight: bold;"
"font-size: 20px;"
"}"
"QPushButton:hover {"
"background: rgba(255, 255, 255, 30);"
"}";

QString revealStyle =
"QPushButton {"
"background-color: rgba(0, 0, 0, 50);"
"color: %1;"
"border-style: outset;"
"border-width: 1px;"
"border-color: rgba(0, 0, 0, 15);"
"padding: 0px;"
"font-weight: bold;"
"font-size: 20px;"
"}";

bool Tile::firstClick = false;
bool Tile::gameOver = false;
int Tile::maxFlags = 0;
int Tile::safeTiles = 0;
int Tile::mineTiles = 0;
int Tile::progress = 0;


Tile::Tile(Position _pos, QWidget* parent = nullptr) :
    QPushButton(parent),
    isMine(false),
    isFlag(false),
    isRevealed(false),
    pos(_pos),
    threats(0)
{
    this->setFixedSize(QSize(35, 35));
    this->setStyleSheet(buttonStyle);
//    connect(this, &QPushButton::released, this, &Tile::released);
    connect(this, &Tile::tileOpened, this, &Tile::setText);
    connect(this, &Tile::floodFill, this, &Tile::floodFiller, Qt::QueuedConnection);
}

Tile::~Tile()
{
    Tile::firstClick = false;
}

void Tile::mousePressEvent(QMouseEvent* event)
{
//    qDebug() << "Released event!";
    if (event->button() == Qt::MiddleButton)
    {
        emit resetGame();
    }
    if (this->isRevealed && event->button() != Qt::MiddleButton)
    {
        return;
    }
    if (Tile::gameOver)
    {
        return;
    }
    if (event->button() == Qt::LeftButton)
    {
        if (!this->isFlag)
        {
            this->revealTile();
        }
    }
    if (event->button() == Qt::RightButton)
    {
        setFlag();
    }
}

void Tile::resetTile()
{
    isMine = false;
    isFlag = false;
    isRevealed = false;
    threats = 0;
    QPushButton::setStyleSheet(buttonStyle);
    QPushButton::setText("");
    QPushButton::setIcon(QIcon());
}

void Tile::setFlag()
{
    if (this->isFlag)
    {
        if (this->isMine)
        {
            ++Tile::mineTiles;
        }
        this->isFlag = false;
        ++Tile::maxFlags;
        QPushButton::setText("");
        emit flagPlaced();
    }
    else
    {
        if (Tile::maxFlags > 0)
        {
            if (this->isMine)
            {
                --Tile::mineTiles;
            }
            this->isFlag = true;
            --Tile::maxFlags;
            Tile::setText();
            emit flagPlaced();
        }
    }
    qDebug() << "Flags: " << Tile::maxFlags << "/" << Tile::mineTiles;
    if (Tile::maxFlags == 0 && Tile::mineTiles == 0)
    {
        if (!Tile::gameOver)
        {
            //qDebug() << "Won by flagging all mines!";
            emit endGame(true);
        }
    }
}

void Tile::revealTile()
{
    if (this->isRevealed)
    {
        return;
    }
    if (!firstClick)
    {
        //qDebug() << "First click at " << QString::number(this->pos.row) << ", " << QString::number(this->pos.col);
        firstClick = true;
        emit firstTile(this);
    }
    if (this->isMine)
    {
        emit endGame(false);
    }
    else
    {
        ++Tile::progress;
    }
    this->isRevealed = true;
    --Tile::safeTiles;
    if (Tile::safeTiles == 0 && !Tile::gameOver)
    {
        //qDebug() << "Won by opening all safe tiles!";
        emit endGame(true);
    }
    if (!this->isMine)
    {
        if (this->threats > 0)
        {
//            this->setText(QString::number(this->threats));
        }
        else // No threats --> floodfill
        {
            emit floodFill(this);
        }
    }
    else
    {
        if (!Tile::gameOver)
        {
            emit endGame(false);
        }
    }
    emit tileOpened();
}

void Tile::floodFiller(Tile* tile)
{
    for (auto neighbour : tile->neighbours)
    {
        if (neighbour->isRevealed)
        {
            continue;
        }
        if (!neighbour->isMine)
        {
            neighbour->revealTile();
        }
        else if (neighbour->threats == 0 && !neighbour->isRevealed)
        {
            emit floodFill(neighbour);
        }
    }
}

QList<Tile*> Tile::getNeighbours()
{
    return this->neighbours;
}

void Tile::placeMine()
{
    this->isMine = true;
}

void Tile::addNeighbour(Tile* tile)
{
    this->neighbours.append(tile);
}

void Tile::setNumber()
{
    for (auto tile : this->neighbours)
    {
        if (tile->isMine)
        {
            ++this->threats;
        }
    }
}

void Tile::setText()
{
    QString color;
    switch (this->threats)
    {
    case 1:
        color = "yellow";
        break;
    case 2:
        color = "lawngreen";
        break;
    case 3:
        color = "red";
        break;
    case 4:
        color = "crimson";
        break;
    case 5:
        color = "mediumvioletred";
        break;
    case 6:
        color = "darkcyan";
        break;
    case 7:
        color = "deeppink";
        break;
    case 8:
        color = "lime";
        break;
    default:
        break;
    }


    QPushButton::setStyleSheet(revealStyle.arg(color));
    if (this->isFlag)
    {
        QPushButton::setStyleSheet(flagStyle);
        QPushButton::setText("!");
    }
    else if (this->isMine)
    {
        QPushButton::setStyleSheet(revealStyle.arg("black"));
//        QPushButton::setText("x");
    }
    else if (this->threats)
    {
        QPushButton::setText(QString::number(this->threats));
    }
}

void Tile::removeText()
{
    QPushButton::setText("");
}

