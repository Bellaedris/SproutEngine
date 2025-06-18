#version 460 core

in vec2 texcoord;

uniform sampler2D brightPass;
uniform int horizontal;

out vec4 FragColor;

// 5 gaussian weights
uniform float weight[5] = float[](0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main() {
    // get the size of one texel in uv space
    vec2 texelSize = 1.f / textureSize(brightPass, 0);
    vec3 col = texture(brightPass, texcoord).xyz * weight[0];

    if(horizontal == 0)
    {
        for (int i = 1; i < 5; i++)
        {
            vec2 offset = vec2(0.f, texelSize.x * i);
            col += texture(brightPass, texcoord + offset).xyz * weight[i];
            col += texture(brightPass, texcoord - offset).xyz * weight[i];
        }
    }
    else
    {
        for (int i = 1; i < 5; i++)
        {
            vec2 offset = vec2(texelSize.y * i, 0.f);
            col += texture(brightPass, texcoord + offset).xyz * weight[i];
            col += texture(brightPass, texcoord - offset).xyz * weight[i];
        }
    }

    FragColor = vec4(col, 1.f);
}
