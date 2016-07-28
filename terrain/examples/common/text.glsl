#ifdef _VERTEX_

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec4 color;
out vec2 uv;
flat out vec3 rgb;

void main() {
    gl_Position = vec4(vertex.xy, 0.0, 1.0);
    uv = vertex.zw;
    rgb = color.rgb;
}

#endif

#ifdef _FRAGMENT_

uniform sampler2D font;

in vec2 uv;
flat in vec3 rgb;
layout(location = 0) out vec4 color;

void main() {
    float v = texture(font, uv).r;
    color.rgb = (1.0 - v) * rgb * 0.25 + v * rgb;
    color.a = 0.6 + 0.4 * v;
}

#endif
