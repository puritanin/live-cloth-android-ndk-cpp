#ifndef _SHADERPROGRAM_H_
#define _SHADERPROGRAM_H_

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

// ----------------------------------------------------------------------------------------------------------------------------

enum
{
	ATTRIB_POSITION,
	ATTRIB_COLOR,
	ATTRIB_NORMAL,
	ATTRIB_TEXCOORD,
	NUM_ATTRIBS
};

enum
{
	UNIFORM_MODELVIEWPROJECTION_MATRIX,
	UNIFORM_POINT_SIZE,
	UNIFORM_TEXTURE,
	NUM_UNIFORMS
};

// ----------------------------------------------------------------------------------------------------------------------------

static const char g_VertexShader1[] =
	"attribute vec4 a_Position;"
	"attribute vec4 a_Color;"
	"uniform mat4 u_ModelViewProjectionMatrix;"
	"uniform float u_PointSize;"
	"varying mediump vec4 v_Color;"
	"void main()"
	"{"
	"    v_Color = a_Color;"
	"    gl_PointSize = u_PointSize;"
	"    gl_Position = u_ModelViewProjectionMatrix * a_Position;"
	"}";

static const char g_FragmentShader1[] =
	//"precision mediump float;"
	"varying mediump vec4 v_Color;"
	"void main()"
	"{"
	"    gl_FragColor = v_Color;"
	"}";

static const char g_VertexShader2[] =
    "attribute vec4 a_Position;"
    "attribute vec2 a_TexCoord;"
    "uniform mat4 u_ModelViewProjectionMatrix;"
    "varying mediump vec2 v_TexCoord;"
    "void main()"
    "{"
    "    v_TexCoord = a_TexCoord;"
    "    gl_Position = u_ModelViewProjectionMatrix * a_Position;"
    "}";

static const char g_FragmentShader2[] =
	//"precision mediump float;"
    "varying mediump vec2 v_TexCoord;"
    "uniform sampler2D u_Texture;"
    "void main()"
    "{"
    "    gl_FragColor = texture2D(u_Texture, v_TexCoord);"
    //"    gl_FragColor = vec4(v_TexCoord, 0.0, 1.0);"
    "}";

// ----------------------------------------------------------------------------------------------------------------------------

class ShaderProgram
{
public:
	ShaderProgram();
	~ShaderProgram();

	bool CreateAndAttachShaders(const char* pVertexSource, const char* pFragmentSource);
	bool Link();

	GLuint m_Program;
	GLuint m_AttribLocations[NUM_ATTRIBS];
	GLuint m_UniformLocations[NUM_UNIFORMS];

private:
	GLuint LoadShader(GLenum shaderType, const char* pSource);

	GLuint m_VertexShader;
	GLuint m_FragmentShader;
};

#endif // _SHADERPROGRAM_H_