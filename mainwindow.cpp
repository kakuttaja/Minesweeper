#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Minesweeper");

    topButton = ui->topButton;
    middleButton = ui->middleButton;
    bottomButton = ui->bottomButton;

    setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);

    connect(topButton, &QPushButton::clicked, this, &MainWindow::startTop);
    connect(middleButton, &QPushButton::clicked, this, &MainWindow::startMid);
    connect(bottomButton, &QPushButton::clicked, this, &MainWindow::startBot);
}

void MainWindow::startTop() {
    //qDebug() << "Top clicked\n";
    game = new Game(10, 10, this);
    game->show();
    this->hide();
}

void MainWindow::startMid() {
    //qDebug() << "Mid clicked\n";
    game = new Game(15, 15, this);
    game->show();
    this->hide();
}

void MainWindow::startBot() {
    //qDebug() << "Bot clicked\n";
    game = new Game(20, 20, this);
    game->show();
    this->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

