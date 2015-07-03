#version 420

in vec4 varyingColor;
uniform vec4 uniformColor;

uniform bool texturedObject;
uniform sampler2D textureSampler;

void main()
{
	if (texturedObject)
	{
		vec2 texCoord = vec2(varyingColor.r, varyingColor.g);
		vec4 textureColor = texture(textureSampler, texCoord);
		gl_FragColor = textureColor * uniformColor;
	}
	
	else
	{
		gl_FragColor = varyingColor * uniformColor;
	}
}