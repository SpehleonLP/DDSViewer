#ifndef GLBLITVAO_H
#define GLBLITVAO_H

class GLViewWidget;

namespace glDefaultVAOs
{
    void AddRef();
    void Release(GLViewWidget * gl);

    void BindVAO(GLViewWidget * gl);

    void RenderPoint(GLViewWidget * gl);
    void RenderSquare(GLViewWidget * gl);
    void RenderCube(GLViewWidget * gl);
}


#endif // GLBLITVAO_H
