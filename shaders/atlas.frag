#version 330 core


// the atlas texture
uniform sampler2D atlasTexture;

in vec2 vertexCoord;

// expecting rasterization to properly interpolate UV
in vec2 texCoord;

out vec4 FragColor;

void main()
{
	float red = texture(atlasTexture, texCoord).r;
	FragColor = vec4(1.0, 1.0, 1.0, 1.0 - red);
	//FragColor = vec4(texCoord.x, texCoord.y, 1.0, 1.0);
	//FragColor = vec4(texCoord.r, texCoord.g, 0, 1.0);
	//FragColor = vec4(texture(atlasTexture, vertexCoord).rrr, 1.0);
}