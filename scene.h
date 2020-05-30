#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLWindow>
#include <QMatrix4x4>
#include <QVector3D>
#include <QSharedPointer>
#include <camera.h>
#include <vector>

//! \brief Класс формирования сцены для просмотра 3D
class Scene : public QOpenGLWidget, protected QOpenGLFunctions
{
  Q_OBJECT

public:
  //! \brief Конструтор
  Scene(const QString& plyFilePath, QWidget* parent = 0);
  //! \brief Деструктор
  ~Scene();


public slots:
  //! \brief Задание размера точек
  void setPointSize(size_t size);
  //! \brief  Прикрепление камеры
  void attachCamera(QSharedPointer<Camera> camera);
  //! \brief Центрирование облака точек
  void centering();



protected:
  //! \brief Инициализация сцены
  void initializeGL() Q_DECL_OVERRIDE;
  //! \brief Рендеринг элементов сцены
  void paintGL() Q_DECL_OVERRIDE;
  //! \brief Изменение размера сцены
  void resizeGL(int width, int height) Q_DECL_OVERRIDE;
  //! \brief Событие перемещения мыши
  void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;


private slots:
  void _onCameraChanged(const CameraState& state);

private:
  void _loadPLY(const QString& plyFilePath);
  void _cleanup();
  void _drawFrameAxis();

  float _pointSize;
  std::vector<std::pair<QVector3D, QColor> > _axesLines;

  QPoint _prevMousePosition;
  QOpenGLVertexArrayObject _vao;
  QOpenGLBuffer _vertexBuffer;
  QScopedPointer<QOpenGLShaderProgram> _shaders;

  QMatrix4x4 _projectionMatrix;
  QMatrix4x4 _cameraMatrix;
  QMatrix4x4 _worldMatrix;

  QVector<float> _pointsData;
  QVector<uint32_t> _facesData;
  size_t _pointsCount;
  size_t _facesCount;
  QVector3D _pointsBoundMin;
  QVector3D _pointsBoundMax;

  QSharedPointer<Camera> _currentCamera;
};
