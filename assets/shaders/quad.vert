#version 430

struct Transform {
    ivec2 atlasOffset; // Offset in the texture atlas
    ivec2 atlasSize;   // Size of the texture in the atlas
    vec2 position;
    vec2 size;

};


layout(std430, binding = 0) buffer TransformSBO {
    Transform transforms[];
};


uniform vec2 screenSize; // Size of the screen, used for normalization

layout (location = 0) out vec2 textureCoordsOut;

void main() {

    // Batch rendering with instancing
    // gl_InstanceID gives us the index of the instance being rendered
    Transform transform = transforms[gl_InstanceID];

    vec2 vertices[6] = 
    {
        transform.position,                                     // Top Left
        vec2(transform.position + vec2(0, transform.size.y)),   // Bottom Left
        vec2(transform.position + vec2(transform.size.x, 0)),   // Top Right
        vec2(transform.position + vec2(transform.size.x, 0)),   // Top Right
        vec2(transform.position + vec2(0, transform.size.y)),   // Bottom Left
        transform.position + transform.size                     // Bottom Right  
    };

    float left = transform.atlasOffset.x;
    float top = transform.atlasOffset.y;
    float right = transform.atlasOffset.x + transform.atlasSize.x;
    float bottom = transform.atlasOffset.y + transform.atlasSize.y;

    vec2 textureCoords[6] = 
    {
        vec2(left, top),
        vec2(left, bottom),
        vec2(right, top),
        vec2(right, top),
        vec2(left, bottom),
        vec2(right, bottom)
    };

    // gl_Position = vec4(vertices[gl_VertexID], 1.0, 1.0);

    {
        vec2 vertexPos = vertices[gl_VertexID];
        vertexPos.y = -vertexPos.y + screenSize.y;
        vertexPos = 2.0 * (vertexPos / screenSize) - 1.0;
        gl_Position = vec4(vertexPos, 0.0, 1.0);
    }

    textureCoordsOut = textureCoords[gl_VertexID];
}