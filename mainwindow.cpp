#include <QMenuBar>
#include <QMenu>
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <QLabel>
#include <QApplication>
#include <QDesktopWidget>

#include "mainwindow.h"
#include "viewer.h"


const QString TITLE = QObject::tr("3D PLY Viewer");


MainWindow::MainWindow()
{
  const QRect desk = QApplication::desktop()->availableGeometry(QApplication::desktop()->screenNumber(this));
  resize(desk.width() * .8, desk.height() * .8);

  setWindowTitle(TITLE);
  setMinimumSize(600, 400);

  QMenuBar *menuBar = new QMenuBar;
  setMenuBar(menuBar);
  QMenu *fileMenu = menuBar->addMenu(tr("&Файл"));
  QAction *openFile = new QAction(tr("&Открыть"), fileMenu);
  fileMenu->addAction(openFile);
  connect(openFile, &QAction::triggered, this, &MainWindow::_openFileDialog);
  QAction *closeView = new QAction(tr("&Закрыть"), fileMenu);
  fileMenu->addAction(closeView);
  connect(closeView, &QAction::triggered, this, &MainWindow::_closeView);


  if (QApplication::arguments().size() > 1) {
    _openView(QApplication::arguments()[1]);
  } else {
    auto welcomeHint = new QLabel();
    welcomeHint->setTextFormat(Qt::RichText);
    QString t = "<font color=gray>";
    t += "<h1><u>Добро пожаловать!</u></h1>";
    t += "<p />";
    t += "<ul>";
    t += "<li>Используйте меню <b>Файл</b> -> <b>Открыть</b> чтобы загрузить PLY файл</li>";
    t += "<li><h2>Советы по навигации</h2></li>";
    t += "<ul>";
    t += "<li>Используйте мышь чтобы вращать камеру</li>";
    t += "<li>Используйте мышь и клавишу <i>SHIFT</i> чтобы перемещать камеру</li>";
    t += "<li>Используйте колесико мыши для масштабирования</li>";
    t += "<li>Используйте клавиши <i>UP, DOWN, LEFT, RIGHT</i> (или <i>W,S,A,D</i>), чтобы приближать, отдалять камеру и перемещать её влево, вправо </li>";
    t += "<li>Используйте клавиши <i>Q, Z</i> (или <i>SPACE, C</i>), чтобы перемещать камеру вверх/вниз</li>";
    t += "</ul>";
    t += "<ul>";
    t += "</ul>";
    t += "<ul>";
    t += "<font>";
    welcomeHint->setText(t);
    welcomeHint->setMargin(50);
    setCentralWidget(welcomeHint);
  }
}


void MainWindow::_openFileDialog()
{
  const QString filePath = QFileDialog::getOpenFileName(this, tr("Открыть PLY файл"), "", tr("PLY Files (*.ply)"));
  if (!filePath.isEmpty()) {
    _openView(filePath);
  }
}


void MainWindow::_openView(const QString& filePath) {
  _closeView();

  try {
    setCentralWidget(new Viewer(filePath));
    setWindowTitle(QString("%1 - %2").arg(filePath).arg(TITLE));
  } catch (const std::exception& e) {
    QMessageBox::warning(this, tr("Невозможно открыть просмотр"), e.what());
  }
}


void MainWindow::_closeView()
{
  if (centralWidget()) {
    centralWidget()->close();
    takeCentralWidget()->deleteLater();
    setWindowTitle(TITLE);
  }
}

