#include "ShaderProgram.h"

// ----------------------------------------------------------------------------------------------------------------------------

ShaderProgram::ShaderProgram()
{
	m_VertexShader = 0;
	m_FragmentShader = 0;
	m_Program = 0;
}

ShaderProgram::~ShaderProgram()
{
	if (m_VertexShader != 0) {
		glDetachShader(m_Program, m_VertexShader);
		glDeleteShader(m_VertexShader);
		m_VertexShader = 0;
	}

	if (m_FragmentShader != 0) {
		glDetachShader(m_Program, m_FragmentShader);
		glDeleteShader(m_FragmentShader);
		m_FragmentShader = 0;
	}

	if (m_Program != 0) {
		glDeleteProgram(m_Program);
		m_Program = 0;
	}
}

bool ShaderProgram::CreateAndAttachShaders(const char* pVertexSource, const char* pFragmentSource)
{
	m_VertexShader = LoadShader(GL_VERTEX_SHADER, pVertexSource);
	m_FragmentShader = LoadShader(GL_FRAGMENT_SHADER, pFragmentSource);

	if (m_VertexShader == 0 || m_FragmentShader == 0) {
		return false;
	}

	m_Program = glCreateProgram();

	glAttachShader(m_Program, m_VertexShader);
	glAttachShader(m_Program, m_FragmentShader);

	return true;
}

bool ShaderProgram::Link()
{
	glLinkProgram(m_Program);

	int linkStatus;
	glGetProgramiv(m_Program, GL_LINK_STATUS, &linkStatus);

	if (linkStatus == GL_FALSE) {
		return false;
	}

	return true;
}

GLuint ShaderProgram::LoadShader(GLenum shaderType, const char* pSource)
{
	GLuint shader = glCreateShader(shaderType);

	glShaderSource(shader, 1, &pSource, NULL);
	glCompileShader(shader);

	int compileStatus;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

	if(compileStatus == GL_FALSE) {
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}
