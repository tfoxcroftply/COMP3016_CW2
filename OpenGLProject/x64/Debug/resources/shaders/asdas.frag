#version 460
out vec4 FragColor;

in vec2 textureCoordinatesFrag;
uniform sampler2D textureIn;
uniform bool applyFilter;
uniform bool isSand;

uniform sampler2D textureMix;
uniform bool mixTextures;

void main()
{
    if (!isSand) {
        vec4 textureColour = texture(textureIn, textureCoordinatesFrag);

        if (mixTextures) {
            vec4 textureMixColour = texture(textureMix, textureCoordinatesFrag);
            textureColour = mix(textureColour, textureMixColour, 0.7);
        }

        if (applyFilter) {
            vec4 blueTintScreen = vec4(0.05, 0.25, 0.4, 1);
            FragColor = mix(textureColour, blueTintScreen, 0.7);
        } else {
            FragColor = textureColour;
        }
    } else {
        FragColor = vec4(0.75, 0.7, 0.5, 1.0);
    }
}