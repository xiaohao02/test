#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <PlayerController.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_playerController = new PlayerController("H:\\download\\ffmpeg-mux-data\\juren.mp4",
                                              ui->widget);

    m_playerController->play();
}

MainWindow::~MainWindow()
{
    delete ui;
}
