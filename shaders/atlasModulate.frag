#version 330 core

// the atlas texture
uniform sampler2D atlasTexture;
uniform vec4 modulation;

// expecting rasterization to properly interpolate UV
in vec2 texCoord;

out vec4 FragColor;

void main()
{
	float red = texture(atlasTexture, texCoord).r;
	FragColor = vec4(modulation.rgb, (1.0 - red) * modulation.a);
}