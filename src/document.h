#ifndef DOCUMENT_H
#define DOCUMENT_H
#include "dds_header.hpp"
#include <glm/vec2.hpp>
#include <memory>
#include <string>
#include <vector>

class MainWindow;
class GLViewWidget;
struct FormatEntry;

class Document
{
public:
	static std::unique_ptr<Document> Factory(std::string path, MainWindow * window);
	~Document();

	void Render(GLViewWidget * gl);

	int width() const { return ddsHeader.dwWidth; }
	int height() const { return ddsHeader.dwHeight; }
	int depth() const { return (ddsHeader.dwFlags & DDSD_DEPTH)? ddsHeader.dwDepth : 1; }

	glm::vec2 GetScreenCenter() const;
	glm::ivec2 GetDimensions() const { return glm::ivec2(ddsHeader.dwWidth, ddsHeader.dwHeight); };

	const char * GetDimension() const;
	const char * GetDxgiFormat() const;

	FormatEntry const* GetFormatInfo() const;

	std::string GetFourCC() const { return std::string((char*)&ddsHeader.ddspf.dwFourCC, 4); };
	int GetPitch() const { return ddsHeader.dwPitchOrLinearSize; };
	int GetBitDepth() const { return ddsHeader.ddspf.dwRGBBitCount; }
	int GetMipMaps() const { return (ddsHeader.dwFlags & DDSD_MIPMAPCOUNT)? ddsHeader.dwMipMapCount : 1; }


	bool IsDX10() const { return ddsHeader.ddspf.dwFourCC == TYPE_DX10;  };

	MainWindow *const m_window{};

private:
	Document(std::ifstream file, MainWindow * window);

	std::vector<uint8_t> GetTextureData(std::ifstream  & file);
	uint32_t UploadTexture(GLViewWidget * gl, std::vector<uint8_t> &&, FormatEntry const&);

	DDS_HEADER       ddsHeader;
	DDS_HEADER_DXT10 dx10Header;
	uint32_t	     texture{};
};

#endif // DOCUMENT_H
