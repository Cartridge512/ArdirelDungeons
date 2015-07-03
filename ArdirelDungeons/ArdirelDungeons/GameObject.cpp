#include "GameObject.h"

using namespace glm;

GameObject::GameObject(void)
	: numVerts(0)
	, floatsPerVert(3)
	, numFloats(0 * 3)
{
}

GameObject::GameObject(GLfloat vertexData[], GLsizei numVerts, GLuint program)
	: numVerts(numVerts)
	, floatsPerVert(5)
	, numFloats(numVerts * 5)
{
	this-> program = program;

	//Get locations of uniform variables
	uniformWorldMatrixLocation = glGetUniformLocation(program, "worldMatrix");
	uniformColorLocation =		 glGetUniformLocation(program, "uniformColor");
	uniformTexturedLocation =    glGetUniformLocation(program, "texturedObject");

	//Vertex Array
	glGenVertexArrays(1, &vaoIndex);
	glBindVertexArray(vaoIndex);

	//Buffers
	glGenBuffers(1, &vboIndex);
	glBindBuffer(GL_ARRAY_BUFFER, vboIndex);

	//Upload X,Y,R,G,B (vertex data)
	glBufferData(	GL_ARRAY_BUFFER,
					sizeof(GL_FLOAT) * numFloats,
					vertexData,
					GL_STATIC_DRAW		);

	// X, Y
	glVertexAttribPointer(	0,
							2,
							GL_FLOAT,
							GL_FALSE,
							sizeof(GL_FLOAT) * floatsPerVert,
							0			);

	glEnableVertexAttribArray(0);
	
	// R, G, B
	glVertexAttribPointer(
		1,								// The index of the attribute - 1 since it's the next one
		3,								// The number of components for this attribute - The next 4 are the color (r,g,b,a)
		GL_FLOAT,						// The type of data, so OpenGL knows how big each component is
		GL_FALSE,						// We don't want our data to be normalized
		sizeof(GL_FLOAT) * floatsPerVert,			// The stride (in bytes) between each of these attributes - 6 since pos (2) + color(4) = 6 total
		(void*)(sizeof(GL_FLOAT) * 2));	// An offset to the beginning of the first one of these attributes, as a pointer (this is ugly, but it works)

	glEnableVertexAttribArray(1);

	velocity = vec3(0, 0, 0);
	objscale = vec3(1, 1, 1);
	rotationAxis = vec3(0, 0, 1);
	rotationAngle = 0; 
	color = vec3(1, 1, 1);
	textured = true;
}

void GameObject::draw()
{
	mat4 worldMatrix =	translate(position) *
						scale(objscale) *
						rotate(rotationAngle, rotationAxis);

	glProgramUniformMatrix4fv(program, uniformWorldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glProgramUniform4f(program, uniformColorLocation, color.r, color.g, color.b, 1);
	glProgramUniform1i(program, uniformTexturedLocation, textured);

	glBindVertexArray(vaoIndex);
	glBindTexture(GL_TEXTURE_2D, texture);

	glDrawArrays(	GL_TRIANGLES,
					0,
					numVerts		);
}

void GameObject::setTex(int texNum)
{
	texture = textures[texNum];
	std::cout<<texture<<std::endl;
	if( 0 == texture )
	{
		printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
	}
}

void GameObject::loadTex(char* file)
{
	texture = SOIL_load_OGL_texture
	         (file, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
	          SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | 
	           SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT );
	if( texture > 0 )
	{
		glEnable( GL_TEXTURE_2D ); 
		glEnable(GL_ALPHA_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}

int GameObject::nlCalc()
{
	int lev = 1;
	if(exp >= 180){lev = 9;}
	else if(exp >= 140){lev = 8;}
	else if(exp >= 105){lev = 7;}
	else if(exp >= 75){lev = 6;}
	else if(exp >= 50){lev = 5;}
	else if(exp >= 30){lev = 4;}
	else if(exp >= 15){lev = 3;}
	else if(exp >= 5){lev = 2;}
	else{lev = 1;}
	cout<<exp<<endl;
	return lev;
}

GameObject::~GameObject(void)
{
}
