#pragma once

#include <QObject>
#include <QVector3D>

//! \brief Структура положения камеры
struct CameraState {
  //! \brief Конструктор положения камеры
  CameraState(const QVector3D& position_, const QVector3D& rotation_,
              double frontClippingDistance_, double farClippingDistance_)
    : position(position_),
      rotation(rotation_),
      frontClippingDistance(frontClippingDistance_),
      rearClippingDistance(farClippingDistance_)
  {}

  const QVector3D position;
  const QVector3D rotation;
  const double frontClippingDistance;
  const double rearClippingDistance;
};

//! \brief Класс камеры
class Camera : public QObject
{
  Q_OBJECT

public:
  //! \brief Шаг вращения
  enum RotationSTEP {RK = 1};

  //! \brief Конструктор по умолчанию
  Camera();
  //! \brief Приближение камеры
  void forward();
  //! \brief Отдаление камеры
  void backward();
  //! \brief Сдвиг камеры влево
  void left();
  //! \brief Сдвиг камеры вправо
  void right();
  //! \brief Сдвиг камеры вверх
  void up();
  //! \brief Сдвиг камеры вниз
  void down();
  //! \brief Задание позиции камере
  void setPosition(const QVector3D& position);
  //! \brief Вращение камеры
  void rotate(int dx, int dy, int dz);
  //! \brief Задание дистанции передней плоскости отсечения
  void setFrontCPDistance(double distance);
  //! \brief Задание дистанции задней плоскости отсечения
  void setRearCPDistance(double distance);
  //! \brief Положение камеры
  CameraState state() const;


signals:
  //! \brief Сигнал изменения положения камеры
  void changed(const CameraState& newState);
  //! \brief Сигнал изменения угла поворота по оси X
  void xRotationChanged(int angle);
  //! \brief Сигнал изменения угла поворота по оси Y
  void yRotationChanged(int angle);
  //! \brief Сигнал изменения угла поворота по оси Z
  void zRotationChanged(int angle);


public slots:
  //! \brief Задание поворота по оси X
  void setXRotation(int angle);
  //! \brief Задание поворота по оси Y
  void setYRotation(int angle);
  //! \brief Задание поворота по оси Z
  void setZRotation(int angle);


private:
  double _frontClippingPlaneDistance;
  double _rearClippingDistance;
  QVector3D _position;
  int _xRotation;
  int _yRotation;
  int _zRotation;

  void _notify() {emit changed(state());}

};
