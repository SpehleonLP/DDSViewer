#include "document.h"
#include "glviewwidget.h"
#include "mainwindow.h"
#include "format_table.hpp"
#include "Shaders/textureshaders.h"
#include "Shaders/defaultvaos.h"
#include <fstream>

std::unique_ptr<Document> Document::Factory(std::string path, MainWindow * window)
{
	std::ifstream file(path);

	if (!file.is_open())
	{
		throw std::system_error(std::make_error_code(std::errc::no_such_file_or_directory));
	}

	file.exceptions ( std::ifstream::failbit | std::ifstream::badbit );

	return std::unique_ptr<Document>(new Document(std::move(file), window));
}


Document::Document(std::ifstream file, MainWindow * window) : m_window(window)
{
	uint32_t dwMagic;
	file.read((char*)&dwMagic, sizeof(dwMagic));
	file.read((char*)&ddsHeader, sizeof(ddsHeader));

	if(ddsHeader.dwSize != sizeof(ddsHeader)
	|| dwMagic != TYPE_DDS)
		throw std::runtime_error("File header is not a DDS file");

	if(IsDX10())
		file.read((char*)&dx10Header, sizeof(dx10Header));

	auto format = GetFormatInfo();

	if(format == nullptr)
		throw std::runtime_error("Unknown file format");

	auto gl = window->GetGl();
	gl->makeCurrent();

	texture = UploadTexture(gl, GetTextureData(file), *format);

	TextureShader::Shader2D.AddRef();
	TextureShader::Shader3D.AddRef();
	glDefaultVAOs::AddRef();
}

Document::~Document()
{
	auto gl = m_window->GetGl();

	TextureShader::Shader2D.Release(gl);
	TextureShader::Shader3D.Release(gl);
	glDefaultVAOs::Release(gl);


	if(texture)
		gl->glDeleteTextures(1, &texture);
}

std::vector<uint8_t> Document::GetTextureData(std::ifstream  & file)
{
	std::vector<uint8_t> texture;

	const std::streampos startPos = file.tellg();
	file.seekg(0, file.end);
	const std::streampos fileSize = file.tellg() - startPos;
	file.seekg(startPos, file.beg);

	if (fileSize < 0)
	{
		throw std::system_error(std::make_error_code(std::errc::io_error));
	}

	texture.resize(static_cast<std::size_t>(fileSize));
	file.read((char*)texture.data(), texture.size());

	return texture;
}

glm::vec2 Document::GetScreenCenter() const
{
	glm::vec2 d = GetDimensions();
	return m_window->GetScroll() * d;
}

const char * Document::GetDimension() const
{
	if(IsDX10())
	{
		switch(dx10Header.resourceDimension)
		{
		default:
			return "UNKNOWN";
		case D3D10_RESOURCE_DIMENSION_BUFFER:
			return "TEXTURE_BUFFER";
		case D3D10_RESOURCE_DIMENSION_TEXTURE1D:
			return "TEXTURE_1D";
		case D3D10_RESOURCE_DIMENSION_TEXTURE2D:
			return "TEXTURE_2D";
		case D3D10_RESOURCE_DIMENSION_TEXTURE3D:
			return "TEXTURE_3D";
		}
	}

	if(ddsHeader.dwHeight == 1)
		return "TEXTURE_1D";

	return "TEXTURE_2D";
}

const char * Document::GetDxgiFormat() const
{
	auto format = GetFormatInfo();
	if(format) return format->name;
	return "UNKNOWN";
}

FormatEntry const* Document::GetFormatInfo() const
{
	if(IsDX10())
	{
		for(uint32_t i = 0; i < NO_ENTIRES; ++i)
		{
			if(g_FormatTable[i].dxFormat == dx10Header.dxgiFormat)
				return &g_FormatTable[i];
		}

		return nullptr;
	}

	if(ddsHeader.ddspf.dwFlags & DDS_FOURCC)
	{
		for(uint32_t i = 0; i < NO_ENTIRES; ++i)
		{
			if(g_FormatTable[i].fourCC == ddsHeader.ddspf.dwFourCC)
				return &g_FormatTable[i];
		}
	}

	return nullptr;
}

void Document::Render(GLViewWidget * gl)
{
	if(depth() == 1)
	{
		gl->glActiveTexture(GL_TEXTURE0);
		gl->glBindTexture(GL_TEXTURE_2D, texture);

		TextureShader::Shader2D.bind(gl, m_window->GetMipLevel(), m_window->GetZScroll());

		glDefaultVAOs::BindVAO(gl);
		glDefaultVAOs::RenderSquare(gl);
	}
	else
	{
		gl->glActiveTexture(GL_TEXTURE0);
		gl->glBindTexture(GL_TEXTURE_3D, texture);

		TextureShader::Shader3D.bind(gl, m_window->GetMipLevel(), m_window->GetZScroll());

		glDefaultVAOs::BindVAO(gl);
		glDefaultVAOs::RenderSquare(gl);
	}
}

static void ApplySamplerSettings(GLViewWidget * gl, uint32_t target,  FormatEntry const& format)
{
//set default reader values...
	gl->glTexParameteri( target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST );
	gl->glTexParameteri( target, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	gl->glTexParameteri( target, GL_TEXTURE_WRAP_S, GL_CLAMP );
	gl->glTexParameteri( target, GL_TEXTURE_WRAP_T, GL_CLAMP );

	if(target == GL_TEXTURE_3D)
		gl->glTexParameteri( target, GL_TEXTURE_WRAP_R, GL_CLAMP );

//	gl->glTexParameteriv(target, GL_TEXTURE_SWIZZLE_RGBA, format.swizzle);

	gl->glAssert();
}

uint32_t Document::UploadTexture(GLViewWidget * gl, std::vector<uint8_t> && data, FormatEntry const& format)
{
	if(data.empty())
		throw std::runtime_error("attempted to upload image which has not been opened.");

	gl->makeCurrent();

//swizzle the input
	int width = this->width();
	int height = this->height();
	int depth = this->depth();
	auto mip_maps = GetMipMaps();

//build texture
	GLuint texture;
	gl->glGenTextures(1, &texture);

	uint8_t const* image = &data[0];

	if(depth == 1)
	{
		gl->glBindTexture(GL_TEXTURE_2D, texture);
		ApplySamplerSettings(gl, GL_TEXTURE_2D, format);
		gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, GetMipMaps()-1);
		gl->glAssert();

		if(format.glType != 0)
		{
			auto pitch   = GetPitch() / width;

			for(int i = 0; i < GetMipMaps(); ++i)
			{
				assert((uint32_t)((image - data.data()) + (pitch*width*height)) <= data.size());

				gl->glTexImage2D(
					GL_TEXTURE_2D,  //target
					i,	//level
					format.glInternalFormat, //internal format
					width, //width
					height, //height,
					0, //border must be 0
					format.glFormat, //format of incoming source
					format.glType, //specific format
					image);

				image += pitch * width * height;
				width  /= 2;
				height /= 2;

				gl->glAssert();
			}
		}
		else
		{
			auto bytes   = GetPitch();

			auto blocks = (width/4) * (height/4);
			auto estimated_bytes = blocks * 8;

	//		assert(bytes == (width*height*8/16));


			for(int i = 0; i < GetMipMaps(); ++i)
			{
				assert((uint32_t)((image - data.data()) + (bytes)) <= data.size());

				gl->glCompressedTexImage2D(
					GL_TEXTURE_2D,  //target
					i,	//level
					format.glInternalFormat, //internal format
					width, //width
					height, //height,
					0, //border must be 0
					bytes, //bytes of data stream.
					image);

				image += bytes;
				width  /= 2;
				height /= 2;
				bytes  /= 4;

				gl->glAssert();
			}
		}

		gl->glBindTexture(GL_TEXTURE_2D, 0);
	}
	else
	{
		gl->glBindTexture(GL_TEXTURE_3D, texture);
		ApplySamplerSettings(gl, GL_TEXTURE_3D, format);
		gl->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, GetMipMaps());
		gl->glAssert();

		if(format.glType != 0)
		{
			auto pitch   = GetPitch() / width;

			for(int i = 0; i < GetMipMaps(); ++i)
			{
				assert((uint32_t)((image - data.data()) + (pitch*width*height*depth)) <= data.size());

				glTexImage3D(
					GL_TEXTURE_3D,  //target
					i,	//level
					format.glInternalFormat, //internal format
					width, //width
					height, //height,
					depth,
					0, //border must be 0
					format.glFormat, //format of incoming source
					format.glType, //specific format
					image);

				image += pitch * width * height * depth;
				width  /= 2;
				height /= 2;

				gl->glAssert();
			}
		}
		else
		{
			auto bytes   = GetPitch();

			for(int i = 0; i < GetMipMaps(); ++i)
			{
				assert((uint32_t)((image - data.data()) + (bytes)) <= data.size());

				gl->glCompressedTexImage3D(
					GL_TEXTURE_3D,  //target
					i,	//level
					format.glInternalFormat, //internal format
					width, //width
					height, //height,
					depth,
					0, //border must be 0
					bytes, //bytes of data stream.
					image);

				image += bytes;
				width  /= 2;
				height /= 2;
				depth  /= 2;
				bytes  /= 8;

				gl->glAssert();
			}
		}

		gl->glBindTexture(GL_TEXTURE_3D, 0);
	}

	gl->glAssert();

	return texture;
}
