#version 120

uniform float pointSize;
uniform mat4 viewMatrix;

attribute vec4 vertex;
attribute float pointRowIndex;

varying float pointIdx;
varying vec3 vert;

void main() {
  gl_Position = viewMatrix * vertex;
  gl_PointSize  = pointSize;
  pointIdx = pointRowIndex;
  vert = vertex.xyz;
}
