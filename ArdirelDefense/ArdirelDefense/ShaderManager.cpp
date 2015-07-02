#include "ShaderManager.h"

char* ShaderManager::loadTextFile(const char* file)
{
	//Open
	ifstream is(file, ios::binary);
	if(!is.is_open()) return 0;
	
	//Length
	is.seekg(0, ios::end);
	int length = (int)is.tellg();
	is.seekg(0, ios::beg);

	//Read
	char* fileContents = new char[length + 1];
	is.read(fileContents, length);
	fileContents[length] = 0;

	//Close
	is.close();

	return fileContents;
}

GLuint ShaderManager::loadShader(const char* file, GLenum shaderType)
{
	//loadTextFile
	const char* shaderCode = loadTextFile(file);
	if (shaderCode == 0)
	{
		cout << "File failed to open: " << file << endl;
		return 0;
	}

	//Compile
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderCode, 0);
	glCompileShader(shader);
	delete[] shaderCode;

	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if(compiled) return shader;
	
	//If it didn't compile...
	GLint logLength;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
	GLchar *log = new GLchar[logLength];
	glGetShaderInfoLog(shader, logLength, 0, log);
	cout << log;
	glDeleteShader(shader);
	delete[] log;

	return 0;
}

GLuint ShaderManager::loadShaderProgram(const char* vertexFile, const char* fragmentFile)
{
	//Vertex
	GLuint vertexShader = loadShader(vertexFile, GL_VERTEX_SHADER);
	if(vertexShader == 0) return 0;

	//Fragment
	GLuint fragmentShader = loadShader(fragmentFile, GL_FRAGMENT_SHADER);
	if(fragmentShader == 0) return 0;

	//Create, Attach & Link
	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (linked) return program;
	
	//If unlinked, print the log
	GLint logLength;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	GLchar *log = new GLchar[logLength];
	glGetProgramInfoLog(program, logLength, 0, log);
	cout << log;
	glDeleteProgram(program);
	delete[] log;
	
	return 0;
}


ShaderManager::ShaderManager(void)
{
}


ShaderManager::~ShaderManager(void)
{
}
