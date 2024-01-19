#version 460
out vec4 FragColor;

in vec2 textureCoordinatesFrag;
uniform sampler2D textureIn;
uniform bool applyFilter;
uniform bool isSand;

void main()
{
    if (!isSand) {
        vec4 texColor = texture(textureIn, textureCoordinatesFrag);
        FragColor = vec4(0.5,0.5,0.5,1.0); // default defined just in case texture doesn't load

        if (applyFilter) {
            vec4 blueTintScreen = vec4(0.05, 0.25, 0.4, 1);
            FragColor = mix(texColor, blueTintScreen, 0.7);
        } else {
            FragColor = texColor;
        }
    } else {
        FragColor = vec4(0.75, 0.7, 0.5, 1.0);
    }
}
