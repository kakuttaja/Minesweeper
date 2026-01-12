#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include "game.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Game* game;
    void startTop();
    void startMid();
    void startBot();
    Ui::MainWindow *ui;
    QPushButton* topButton;
    QPushButton* middleButton;
    QPushButton* bottomButton;
};
#endif // MAINWINDOW_H
