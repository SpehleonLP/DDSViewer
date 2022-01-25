#ifndef GLPROGRAM_H
#define GLPROGRAM_H
#include <memory>
#include <array>
#include <cstdint>
#include <vector>
#include <mutex>

class GLViewWidget;

class glProgram
{
typedef std::mutex mutex_t;
typedef std::lock_guard<mutex_t> lock_guard_t;

typedef GLViewWidget OpenGL;
typedef uint32_t GLuint;
typedef int32_t GLint;
private:
    std::mutex m_mutex;
    int m_refCount;
    GLuint m_program;

    std::array<GLuint,6>  shaders;

    static GLuint compileShader(OpenGL * gl, const char * filename, GLuint);
    static GLuint loadShader(OpenGL * gl, const char * filename, GLuint);
    static int getShaderIndex(GLuint);
    void printLog(OpenGL * gl);

protected:
    void create(OpenGL * gl);
    void compile(OpenGL * gl, const char * text, GLuint);
    void load(OpenGL * gl, const char * filename, GLuint);
    bool tryLoad(OpenGL * gl, const char * filename, GLuint type);
    void attribute(OpenGL * gl, GLuint index, const char * name);
    void link(OpenGL * gl);
    void uniform(OpenGL * gl, GLint & uniform, const char * name);
    bool uniformBlock(OpenGL * gl, uint32_t binding, const char * name);

public:
    bool isGood() const { return m_program; }

    glProgram();
    virtual ~glProgram();

    glProgram(const glProgram & it) = delete;
    glProgram(glProgram && it) = delete;
    glProgram & operator=(const glProgram & it) = delete;
    glProgram & operator=(glProgram &&) = delete;

    virtual void construct(OpenGL * gl) = 0;
    void destruct(OpenGL * gl);

    void AddRef();
    void Release(OpenGL*);

    bool validate(OpenGL * gl);

    bool bindShader(OpenGL * gl);
};

#endif // GLSHADER_H
