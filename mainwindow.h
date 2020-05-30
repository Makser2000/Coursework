#pragma once

#include <QMainWindow>

//! \brief Класс основного окна
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //! \brief Конструктор по умолчанию
    MainWindow();

protected slots:
  //! \brief Открытие окна для нахождения PLY файла
  void _openFileDialog();
  //! \brief Открытие окна просмотра PLY файла
  void _openView(const QString& plyPath);
  //! \brief Закрытие окна просмотра
  void _closeView();
};

