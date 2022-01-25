#include "textureshaders.h"
#include "src/glviewwidget.h"

#define SHADER(k) "#version 150\n" #k
static const char * kVert2D();
static const char * kVert3D();
static const char * kFrag2D();
static const char * kFrag3D();

TextureShader TextureShader::Shader2D{2};
TextureShader TextureShader::Shader3D{3};

void TextureShader::construct(GLViewWidget* gl)
{
    compile(gl, id == 2? kVert2D() : kVert3D(), GL_VERTEX_SHADER);
    compile(gl, id == 2? kFrag2D() : kFrag3D(), GL_FRAGMENT_SHADER);
    attribute(gl, 0, "a_vertex");
    attribute(gl, 3, "a_uv");
    link(gl);
	uniform(gl, u_texture, "u_texture");
	uniform(gl, u_mipMap, "u_mipMap");
	uniform(gl, u_zScroll, "u_zScroll");
}

void TextureShader::bind(GLViewWidget* gl, float mipMap, float zScroll)
{
    if(bindShader(gl))
	{
        gl->glDisable(GL_DEPTH_TEST);
        gl->glDepthMask(GL_FALSE);
        gl->glEnable(GL_BLEND);
        gl->glDisable(GL_CULL_FACE);
	}

    gl->glAssert();
	gl->glUniform1i(u_texture, 0);
	gl->glUniform1f(u_mipMap, mipMap);
	gl->glUniform1f(u_zScroll, zScroll);
}


static const char * kVert2D()
{
	return SHADER(
		layout(std140) uniform Matrices
		{
			mat4  u_projection;
			mat4  u_modelview;
			ivec4 u_screenSize;
			float u_ctime;
		};

		uniform sampler2D u_texture;

		in vec3 a_vertex;
		in vec2 a_uv;

		out vec2 v_uv;

		void main()
		{
			gl_Position = u_projection * (u_modelview * vec4(a_uv * textureSize(u_texture, 0), 0, 1.0));
			v_uv        = a_uv;
		});
}

static const char * kVert3D()
{
	return SHADER(
		layout(std140) uniform Matrices
		{
			mat4  u_projection;
			mat4  u_modelview;
			ivec4 u_screenSize;
			float u_ctime;
		};

		uniform sampler3D u_texture;
		uniform float u_zScroll;

		in vec3 a_vertex;
		in vec2 a_uv;

		out vec3 v_uv;

		void main()
		{
			gl_Position = u_projection * (u_modelview * vec4(a_uv * textureSize(u_texture, 0).xy, 0, 1.0));
			v_uv        = vec3(a_uv, u_zScroll /  textureSize(u_texture, 0).z);
		});
}


static const char * kFrag2D()
{
	return SHADER(
		uniform sampler2D u_texture;
		uniform float u_zScroll;
		uniform float u_mipMap;

		in vec2 v_uv;

		out vec4 FragOut;

		void main()
		{
			FragOut = textureLod(u_texture, v_uv, u_mipMap);
		});
}

static const char * kFrag3D()
{
	return SHADER(
		uniform sampler3D u_texture;
		uniform float u_mipMap;

		in vec3 v_uv;

		out vec4 FragOut;

		void main()
		{
			FragOut = textureLod(u_texture, v_uv, u_mipMap);
		});
}
