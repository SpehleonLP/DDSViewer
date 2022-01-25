#include "transparencyshader.h"
#include "defaultvaos.h"
#include "src/glviewwidget.h"

#define SHADER(k) "#version 150\n" #k
static const char * kVert();
static const char * kFrag();

TransparencyShader TransparencyShader::Shader;

void TransparencyShader::construct(GLViewWidget* gl)
{
    compile(gl, kVert(), GL_VERTEX_SHADER);
    compile(gl, kFrag(), GL_FRAGMENT_SHADER);
    attribute(gl, 0, "a_vertex");
    attribute(gl, 3, "a_uv");
    link(gl);
}


void TransparencyShader::bind(GLViewWidget* gl)
{
    if(bindShader(gl))
	{
        gl->glDisable(GL_DEPTH_TEST);
        gl->glDepthMask(GL_FALSE);
        gl->glDisable(GL_BLEND);
        gl->glDisable(GL_CULL_FACE);
	}

    gl->glAssert();
}

static const char * kVert()
{
	return SHADER(
		layout(std140) uniform Matrices
		{
			mat4  u_projection;
			mat4  u_modelview;
			ivec4 u_screenSize;
			float u_ctime;
		};

		in vec3 a_vertex;
		in vec2 a_uv;

		out vec2 v_uv;

		void main()
		{
			gl_Position = u_projection * (u_modelview * vec4(a_vertex, 1.0));
			v_uv        = a_uv;
		});
}

static const char * kFrag()
{
	return SHADER(
		in vec2 v_uv;
		in vec4 gl_FragCoord;

		out vec4 frag_color;

		void main()
		{
			ivec2 coords = ivec2(gl_FragCoord);
			int tile = (int(coords.x / 10) & 0x01) ^ (int(coords.y / 10) & 0x01);
			float color = (125 + 100 * tile) / 256.f;
			frag_color = vec4(vec3(color), 1.0);
		});
}

