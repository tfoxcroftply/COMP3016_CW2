#version 460
out vec4 FragColor; // Sends

//Texture coordinates from last stage
in vec2 textureCoordinatesFrag;
//Texture (colours)
uniform sampler2D textureIn;

void main()
{
    //Setting of texture & its coordinates as colour map
    FragColor = texture(textureIn, textureCoordinatesFrag);
}