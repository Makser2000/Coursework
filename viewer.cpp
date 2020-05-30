#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QPushButton>
#include <QDesktopWidget>
#include <QApplication>
#include <QMessageBox>
#include <QComboBox>
#include <QTimer>
#include <QGroupBox>
#include <QCheckBox>
#include <QSlider>

#include "camera.h"
#include "scene.h"
#include "viewer.h"

#include <cassert>



QSlider* createAnglecontrolSlider()
{
  QSlider* slider = new QSlider(Qt::Horizontal);
  slider->setRange(0, 360 * Camera::RK);
  slider->setSingleStep(Camera::RK);
  slider->setPageStep(10 * Camera::RK);
  return slider;
}


Viewer::Viewer(const QString& filePath)
{
  setFocusPolicy(Qt::StrongFocus);
  setFocus();

  _scene = new Scene(filePath);

  _camera = QSharedPointer<Camera>(new Camera());
  _scene->attachCamera(_camera);

  auto xSlider = createAnglecontrolSlider();
  auto ySlider = createAnglecontrolSlider();
  auto zSlider = createAnglecontrolSlider();
  connect(xSlider, SIGNAL(valueChanged(int)), _camera.data(), SLOT(setXRotation(int)));
  connect(_camera.data(), SIGNAL(xRotationChanged(int)), xSlider, SLOT(setValue(int)));
  connect(ySlider, SIGNAL(valueChanged(int)), _camera.data(), SLOT(setYRotation(int)));
  connect(_camera.data(), SIGNAL(yRotationChanged(int)), ySlider, SLOT(setValue(int)));
  connect(zSlider, SIGNAL(valueChanged(int)), _camera.data(), SLOT(setZRotation(int)));
  connect(_camera.data(), SIGNAL(zRotationChanged(int)), zSlider, SLOT(setValue(int)));


  auto pointSizeSlider = new QSlider(Qt::Horizontal);
  pointSizeSlider->setRange(1, 20);
  pointSizeSlider->setTickPosition(QSlider::TicksRight);
  pointSizeSlider->setTickInterval(1);
  pointSizeSlider->setPageStep(1);
  connect(pointSizeSlider, &QSlider::valueChanged, this, &Viewer::_updatePointSize);

  auto btnCentering = new QPushButton(tr("Центрирование"));
  btnCentering->setMaximumWidth(100);
  connect(btnCentering, &QPushButton::pressed, [=]() {
      _scene->centering();
  });

  _lblColorBy = new QLabel();

  QWidget* cpWidget = new QWidget();
  QVBoxLayout* controlPanel = new QVBoxLayout();
  cpWidget->setMaximumWidth(300);
  cpWidget->setLayout(controlPanel);
  controlPanel->addWidget(_lblColorBy);
  controlPanel->addWidget(pointSizeSlider);
  controlPanel->addSpacing(20);
  controlPanel->addWidget(new QLabel(tr("Углы поворота камеры")));
  controlPanel->addWidget(xSlider);
  controlPanel->addWidget(ySlider);
  controlPanel->addWidget(zSlider);
  controlPanel->addSpacing(20);
  controlPanel->addWidget(btnCentering);
  controlPanel->addSpacing(20);
  controlPanel->addStretch(2);


  QHBoxLayout *mainLayout = new QHBoxLayout;
  mainLayout->addWidget(_scene);
  mainLayout->addWidget(cpWidget);
  setLayout(mainLayout);

  xSlider->setValue(0);
  ySlider->setValue(0);
  zSlider->setValue(0);

  _updatePointSize(1);

  _camera->setPosition(QVector3D(0, -0.1, -0.2));
  _camera->rotate(0, 50, 0);
  _camera->setRearCPDistance(1);
  _camera->setFrontCPDistance(0);
}


void Viewer::wheelEvent(QWheelEvent* e) {
  if (e->angleDelta().y() > 0) {
    _camera->forward();
  } else {
    _camera->backward();
  }
}


void Viewer::keyPressEvent(QKeyEvent* keyEvent) {
  switch ( keyEvent->key() )
  {
    case Qt::Key_Escape:
      QApplication::instance()->quit();
      break;

    case Qt::Key_Left:
    case Qt::Key_A:
      _camera->left();
      break;

    case Qt::Key_Right:
    case Qt::Key_D:
      _camera->right();
      break;

    case Qt::Key_Up:
    case Qt::Key_W:
      _camera->forward();
      break;

    case Qt::Key_Down:
    case Qt::Key_S:
      _camera->backward();
      break;

    case Qt::Key_Space:
    case Qt::Key_Q:
      _camera->up();
      break;

    case Qt::Key_C:
    case Qt::Key_Z:
      _camera->down();
      break;

    default:
      QWidget::keyPressEvent(keyEvent);
  }
}


void Viewer::_updatePointSize(int value) {
  _scene->setPointSize(value);
  _lblColorBy->setText(QString("Размер точки: %1").arg(value));
}
