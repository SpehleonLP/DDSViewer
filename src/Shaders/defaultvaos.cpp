#include "defaultvaos.h"
#include "src/glviewwidget.h"
#include <glm/gtc/type_precision.hpp>
#include <atomic>
#include <vector>
#include <mutex>

namespace glDefaultVAOs
{


struct DefaultVAOs
{
	GLuint vao{};
	GLuint vbo{};

	mutable std::mutex       mutex;
	mutable std::atomic<int> refCount{0};
};

static DefaultVAOs s_BlitStruct;

void AddRef()
{
	++s_BlitStruct.refCount;
}

void Release(GLViewWidget * gl)
{
	if(--s_BlitStruct.refCount && s_BlitStruct.vao)
	{
		std::lock_guard<std::mutex> lock(s_BlitStruct.mutex);

		if(s_BlitStruct.refCount == 0 && s_BlitStruct.vao)
		{
            gl->glDeleteVertexArrays(1, &s_BlitStruct.vao);
            gl->glDeleteBuffers(1, &s_BlitStruct.vbo);

			s_BlitStruct.vao = 0;
			s_BlitStruct.vbo = 0;
		}
	}
}

void RenderPoint(GLViewWidget * gl)
{
    gl->glDrawArrays(GL_POINTS, 0, 1);
}

void RenderSquare(GLViewWidget * gl)
{
    gl->glDrawArrays(GL_TRIANGLES, 1, 6);
}

void RenderCube(GLViewWidget * gl)
{
    gl->glDrawArrays(GL_TRIANGLES, 7, 36);
}

struct gltfVertex
{
	glm::i8vec4 position;
	glm::i8vec4 normal;
	glm::i8vec4 texCoord0;
};

void BindVAO(GLViewWidget * gl)
{
	assert(s_BlitStruct.refCount != 0);

	if(s_BlitStruct.vao)
	{
        gl->glBindVertexArray(s_BlitStruct.vao);
		return;
	}

	std::lock_guard<std::mutex> lock(s_BlitStruct.mutex);
	assert(s_BlitStruct.refCount != 0);

	const glm::i8vec4 positions[] =
	{
		{ -1,-1, 0, 1 },  {  1,-1, 0, 1 },  { -1, 1, 0, 1 },
		{ -1, 1, 0, 1 },  {  1,-1, 0, 1 },  {  1, 1, 0, 1 },

		{ -1,-1,-1, 1 },  { -1,-1, 1, 1 },  { -1, 1, 1, 1 },
		{  1, 1,-1, 1 },  { -1,-1,-1, 1 },  { -1, 1,-1, 1 },

		{  1,-1, 1, 1 },  { -1,-1,-1, 1 },  {  1,-1,-1, 1 },
		{  1, 1,-1, 1 },  {  1,-1,-1, 1 },  { -1,-1,-1, 1 },

		{ -1,-1,-1, 1 },  { -1, 1, 1, 1 },  { -1, 1,-1, 1 },
		{  1,-1, 1, 1 },  { -1,-1, 1, 1 },  { -1,-1,-1, 1 },

		{ -1, 1, 1, 1 },  { -1,-1, 1, 1 },  {  1,-1, 1, 1 },
		{  1, 1, 1, 1 },  {  1,-1,-1, 1 },  {  1, 1,-1, 1 },

		{  1,-1,-1, 1 },  {  1, 1, 1, 1 },  {  1,-1, 1, 1 },
		{  1, 1, 1, 1 },  {  1, 1,-1, 1 },  { -1, 1,-1, 1 },

		{  1, 1, 1, 1 },  { -1, 1,-1, 1 },  { -1, 1, 1, 1 },
		{  1, 1, 1, 1 },  { -1, 1, 1, 1 },  {  1,-1, 1, 1 },
	};

	std::vector<gltfVertex> vert_array;
	vert_array.resize(43);
	vert_array[0].normal = glm::i8vec4(0, 0, 127, 0);

	auto verts = &vert_array[1];
	for(int tri = 0; tri < 14; ++tri)
	{
		glm::ivec4 p0 = positions[tri*3+0];
		glm::ivec4 p1 = positions[tri*3+1];
		glm::ivec4 p2 = positions[tri*3+2];

		glm::ivec4 u = p1 - p0;
		glm::ivec4 v = p2 - p0;

		glm::ivec4 normal(
			u.y*v.z - u.z*v.y,
			u.z*v.x - u.x*v.z,
			u.x*v.y - u.y*v.x,
			0);

		for(int i = 0; i < 3; ++i)
		{
			verts[tri*3+i].position = positions[tri*3+i];
			verts[tri*3+i].normal   = normal;
			verts[tri*3+i].texCoord0= (positions[tri*3+i] + (int8_t)1) / (int8_t)2;
		}
	}

    gl->glGenVertexArrays(1, &s_BlitStruct.vao);
    gl->glGenBuffers(1, &s_BlitStruct.vbo);

    gl->glBindVertexArray(s_BlitStruct.vao);
    gl->glBindBuffer(GL_ARRAY_BUFFER, s_BlitStruct.vbo);

    gl->glBufferData(GL_ARRAY_BUFFER, vert_array.size() * sizeof(gltfVertex), &vert_array[0], GL_STATIC_DRAW);

	//position
    gl->glVertexAttribPointer(0, 3, GL_BYTE, GL_FALSE, sizeof(gltfVertex), (void*) offsetof(gltfVertex, position));
	//normal
    gl->glVertexAttribPointer(1, 3, GL_BYTE, GL_FALSE, sizeof(gltfVertex), (void*) offsetof(gltfVertex, normal));
//	glVertexAttribPointer(2, 4, GL_BYTE, GL_FALSE, sizeof(gltfVertex), (void*) 8);
	//texcoord0
    gl->glVertexAttribPointer(3, 2, GL_BYTE, GL_FALSE, sizeof(gltfVertex), (void*) offsetof(gltfVertex, texCoord0));

    gl->glEnableVertexAttribArray(0);
    gl->glEnableVertexAttribArray(1);
    gl->glEnableVertexAttribArray(3);
}


}
