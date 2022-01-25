#ifndef TEXTURESHADERS_H
#define TEXTURESHADERS_H
#include "glprogram.h"


class TextureShader  : public glProgram
{
public:
static TextureShader Shader2D;
static TextureShader Shader3D;

	TextureShader(int id) : id(id) {}

	void bind(GLViewWidget* gl, float mipMap, float zScroll);

private:
    void construct(GLViewWidget* gl);

	int16_t id;
	int32_t u_texture;
	int32_t u_zScroll;
	int32_t u_mipMap;
};

#endif // TEXTURESHADERS_H
