#ifndef TRANSPARENCYSHADER_H
#define TRANSPARENCYSHADER_H
#include "glprogram.h"

class TransparencyShader : public glProgram
{
public:
static TransparencyShader Shader;
    void bind(GLViewWidget* gl);

private:
    void construct(GLViewWidget* gl);
};

#endif // TRANSPARENCYSHADER_H
