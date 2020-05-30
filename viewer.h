#pragma once

#include <QWidget>
#include <QVector3D>
#include <QSharedPointer>
#include <QLabel>

#include "camera.h"

class Scene;
//! \brief Класс для просмотра сцены
class Viewer : public QWidget
{
  Q_OBJECT

public:
  //! \brief Конструктор
  Viewer(const QString& filePath);


protected:
  //! \brief Событие вращания колесика мыши
  void wheelEvent(QWheelEvent *);
  //! \brief Событие нажатие на клавишу
  void keyPressEvent(QKeyEvent *);


private slots:
  void _updatePointSize(int);


private:

  Scene* _scene;
  QSharedPointer<Camera> _camera;
  QLabel* _lblColorBy;

};
