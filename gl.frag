#version 330 core

in vec2 outCoord;
uniform sampler2D tex_y;
uniform sampler2D tex_u;
uniform sampler2D tex_v;

out vec4 fragColor;

void main(void)
{
    // 采样 Y, U, V 三个分量的像素值
    // .r 表示读取单通道数据
    float y = texture(tex_y, outCoord).r;
    float u = texture(tex_u, outCoord).r - 0.5;
    float v = texture(tex_v, outCoord).r - 0.5;

    // YUV 转 RGB 矩阵 (BT.601 标准)
    vec3 rgb;
    rgb.r = y + 1.403 * v;
    rgb.g = y - 0.34414 * u - 0.71414 * v;
    rgb.b = y + 1.772 * u;

    fragColor = vec4(rgb, 1.0);
}
