#version 430

layout (location = 0) in vec2 textureCoordsIn;


layout (location = 0) out vec4 fragColor;


layout (location = 0) uniform sampler2D textureAtlas;

void main() {

    vec4 textureColor = texelFetch(textureAtlas, ivec2(textureCoordsIn), 0);

    if (textureColor.a == 0.0) {
        discard; // Discard fragments with low alpha
    }

    fragColor = textureColor;
}