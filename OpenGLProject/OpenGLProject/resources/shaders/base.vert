#version 460
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 textureCoordinatesVertex;

out vec2 textureCoordinatesFrag;
uniform mat4 mvpIn;

void main()
{
    gl_Position = mvpIn * vec4(position.x, position.y, position.z, 1.0);
    textureCoordinatesFrag = textureCoordinatesVertex;
}