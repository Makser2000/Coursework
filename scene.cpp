#include "scene.h"

#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <QScopedPointer>

#include <cmath>
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <limits>

const size_t POINT_STRIDE = 4;

const size_t LINE_STRIDE = 2;


Scene::Scene(const QString& plyFilePath, QWidget* parent)
  : QOpenGLWidget(parent),
    _pointSize(1)
{
  _loadPLY(plyFilePath);
  setMouseTracking(true);

  _axesLines.push_back(std::make_pair(QVector3D(0.0, 0.0, 0.0), QColor(1.0, 0.0, 0.0)));
  _axesLines.push_back(std::make_pair(QVector3D(1.0, 0.0, 0.0), QColor(1.0, 0.0, 0.0)));
  _axesLines.push_back(std::make_pair(QVector3D(0.0, 0.0, 0.0), QColor(0.0, 1.0, 0.0)));
  _axesLines.push_back(std::make_pair(QVector3D(0.0, 1.0, 0.0), QColor(0.0, 1.0, 0.0)));
  _axesLines.push_back(std::make_pair(QVector3D(0.0, 0.0, 0.0), QColor(0.0, 0.0, 1.0)));
  _axesLines.push_back(std::make_pair(QVector3D(0.0, 0.0, 1.0), QColor(0.0, 0.0, 1.0)));

}


void Scene::_loadPLY(const QString& plyFilePath) {

  std::fstream is;
  is.open(plyFilePath.toStdString().c_str(), std::fstream::in);

  std::string line;
  std::getline(is, line);
  if (line != "ply") {
    throw std::runtime_error("не ply формат");
  }

  std::getline(is, line);
  std::stringstream ss(line);
  std::string tag1, tag2;
  ss >> tag1 >> tag2;
  if (tag1 != "format" || tag2 != "ascii") {
      throw std::runtime_error("Не ascii формат");
  }

  _pointsCount = 0;
  while (is.good()) {
    std::getline(is, line);
    std::stringstream ss(line);
    std::string ayf;
    ss>>ayf;
    if (ayf == "end_header") {
      break;
    } else {
      std::stringstream ss(line);
      std::string tag1, tag2, tag3;
      ss >> tag1 >> tag2 >> tag3;
      if (tag1 == "element" && tag2 == "vertex") {
        _pointsCount = std::atoll(tag3.c_str());
      }
      if (tag1== "element" && tag2 =="face"){
          _facesCount=std::atoll(tag3.c_str());
      }
    }
  }

  if (_pointsCount > 0) {
    _pointsData.resize(_pointsCount * POINT_STRIDE);

    std::stringstream ss;
    std::string line;
    float *p = _pointsData.data();
    for (size_t i = 0; is.good() && i < _pointsCount; ++i) {
      std::getline(is, line);
      std::stringstream ss(line);
      float x, y, z;
      ss >> x >> y >> z;

      *p++ = x;
      *p++ = y;
      *p++ = z;
      *p++ = i;

      _pointsBoundMax[0] = std::max(x, _pointsBoundMax[0]);
      _pointsBoundMax[1] = std::max(y, _pointsBoundMax[1]);
      _pointsBoundMax[2] = std::max(z, _pointsBoundMax[2]);
      _pointsBoundMin[0] = std::min(x, _pointsBoundMin[0]);
      _pointsBoundMin[1] = std::min(y, _pointsBoundMin[1]);
      _pointsBoundMin[2] = std::min(z, _pointsBoundMin[2]);
    }

    if (p - _pointsData.data() < _pointsData.size()) {
      throw std::runtime_error("Невозможно правильно считать данные");
    }
  }

  if (_facesCount > 0){
      _facesData.resize(_facesCount * LINE_STRIDE);

      std::stringstream ss;
      std::string line;
      uint32_t *p = _facesData.data();
      for(size_t i = 0; is.good() && i < _facesCount; ++i) {
          std::getline(is, line);
          std::stringstream ss(line);
          int c, ind1, ind2;
          ss >> c >> ind1 >> ind2;
          *p++=ind1;
          *p++=ind2;
      }

      if (p - _facesData.data() < _facesData.size()) {
          throw std::runtime_error("Невозможно правильно считать данные");
      }

  }
}


Scene::~Scene()
{
  _cleanup();
}


void Scene::_cleanup()
{
  makeCurrent();
  _vertexBuffer.destroy();
  _shaders.reset();
  doneCurrent();
}


void Scene::initializeGL()
{
  connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &Scene::_cleanup);

  initializeOpenGLFunctions();
  glClearColor(0.30, 0.4, 0.30, 1);

  _worldMatrix.setToIdentity();

  _shaders.reset(new QOpenGLShaderProgram());
  auto vsLoaded = _shaders->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vertex_shader.glsl");
  auto fsLoaded = _shaders->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fragment_shader.glsl");
  assert(vsLoaded && fsLoaded);
  _shaders->bindAttributeLocation("vertex", 0);
  _shaders->bindAttributeLocation("pointRowIndex", 1);

  _shaders->bind();
  _shaders->setUniformValue("pointsCount", static_cast<GLfloat>(_pointsCount));
  _shaders->link();
  _shaders->release();

  _vao.create();
  QOpenGLVertexArrayObject::Binder vaoBinder(&_vao);
  _vertexBuffer.create();
  _vertexBuffer.bind();
  _vertexBuffer.allocate(_pointsData.constData(), _pointsData.size() * sizeof(GLfloat));
  QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
  f->glEnableVertexAttribArray(0);
  f->glEnableVertexAttribArray(1);
  f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat) + sizeof(GLfloat), 0);
  f->glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat) + sizeof(GLfloat), reinterpret_cast<void *>(3*sizeof(GLfloat)));
  _vertexBuffer.release();
}


void Scene::paintGL()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

  const CameraState camera = _currentCamera->state();
  _cameraMatrix.setToIdentity();
  _cameraMatrix.translate(camera.position.x(), camera.position.y(), camera.position.z());
  _cameraMatrix.rotate(camera.rotation.x(), 1, 0, 0);
  _cameraMatrix.rotate(camera.rotation.y(), 0, 1, 0);
  _cameraMatrix.rotate(camera.rotation.z(), 0, 0, 1);

  glEnable(GL_CLIP_PLANE1);
  glEnable(GL_CLIP_PLANE2);
  const double rearClippingPlane[] = {0., 0., -1., camera.rearClippingDistance};
  glClipPlane(GL_CLIP_PLANE1 , rearClippingPlane);
  const double frontClippingPlane[] = {0., 0., 1., camera.frontClippingDistance};
  glClipPlane(GL_CLIP_PLANE2 , frontClippingPlane);

  QOpenGLVertexArrayObject::Binder vaoBinder(&_vao);
  const auto viewMatrix = _projectionMatrix * _cameraMatrix * _worldMatrix;
  _shaders->bind();
  _shaders->setUniformValue("pointsCount", static_cast<GLfloat>(_pointsCount));
  _shaders->setUniformValue("viewMatrix", viewMatrix);
  _shaders->setUniformValue("pointSize", _pointSize);
  _shaders->setUniformValue("pointsBoundMin", _pointsBoundMin);
  _shaders->setUniformValue("pointsBoundMax", _pointsBoundMax);
  glDrawArrays(GL_POINTS, 0, _pointsData.size());
  glDrawElements(GL_LINES, _facesCount * LINE_STRIDE , GL_UNSIGNED_INT, _facesData.data());
  _shaders->release();

  _drawFrameAxis();

}

void Scene::_drawFrameAxis() {
  glBegin(GL_LINES);
  QMatrix4x4 mvMatrix = _cameraMatrix * _worldMatrix;
  mvMatrix.scale(0.05);
  for (auto vertex : _axesLines) {
    const auto translated = _projectionMatrix * mvMatrix * vertex.first;
    glColor3f(vertex.second.red(), vertex.second.green(), vertex.second.blue());
    glVertex3f(translated.x(), translated.y(), translated.z());
  }
  glEnd();
}

void Scene::centering() {
    QVector3D p((_pointsBoundMax[0] + _pointsBoundMin[0])/2, (_pointsBoundMax[1] + _pointsBoundMin[1])/2, (_pointsBoundMax[2] + _pointsBoundMin[2])/2);
    double r(sqrt((_pointsBoundMax[0] - p.x())* (_pointsBoundMax[0] - p.x()) + (_pointsBoundMax[0] - p.y()) * (_pointsBoundMax[1] - p.y()) + (_pointsBoundMax[2] - p.z()) * (_pointsBoundMax[2] - p.z())));
    float fDistance(r/0.37f);
    double dNear = fDistance - r;
    double dFar = fDistance + r;
    _projectionMatrix.setToIdentity();
    _projectionMatrix.frustum(-r, +r, +r, -r, dNear, dFar);
   // _projectionMatrix.perspective(20.0f,GLfloat(r)/r,dNear,dFar);
    _cameraMatrix.setToIdentity();
    _cameraMatrix.lookAt(QVector3D(0.0f,0.0f,fDistance),p,QVector3D(0.0f,1.0f,0.0f));
}

void Scene::resizeGL(int w, int h)
{
  _projectionMatrix.setToIdentity();
  _projectionMatrix.perspective(70.0f, GLfloat(w) / h, 0.01f, 100.0f);
}

void Scene::mouseMoveEvent(QMouseEvent *event)
{
  const int dx = event->x() - _prevMousePosition.x();
  const int dy = event->y() - _prevMousePosition.y();
  const bool panningMode = (event->modifiers() & Qt::ShiftModifier);
  _prevMousePosition = event->pos();

  if (event->buttons() & Qt::LeftButton) {

    if (panningMode) {
      if (dx > 0) {
        _currentCamera->right();
      }
      if (dx < 0) {
        _currentCamera->left();
      }
      if (dy > 0) {
        _currentCamera->down();
      }
      if (dy < 0) {
        _currentCamera->up();
      }
    } else {
      _currentCamera->rotate(dy, dx, 0);
    }
  }
}

void Scene::setPointSize(size_t size) {
  assert(size > 0);
  _pointSize = size;
  update();
}

void Scene::attachCamera(QSharedPointer<Camera> camera) {
  if (_currentCamera) {
    disconnect(_currentCamera.data(), &Camera::changed, this, &Scene::_onCameraChanged);
  }
  _currentCamera = camera;
  connect(camera.data(), &Camera::changed, this, &Scene::_onCameraChanged);
}

void Scene::_onCameraChanged(const CameraState&) {
  update();
}
