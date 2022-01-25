#ifndef FORMAT_TABLE_HPP
#define FORMAT_TABLE_HPP
#include "dds_header.hpp"
#include <QOpenGLFunctions_3_2_Core>

struct FormatEntry
{
	unsigned int dxFormat;
	unsigned int fourCC;
	unsigned int glInternalFormat;
	unsigned int glFormat;
	unsigned int glType;
	const char * name;
	int swizzle[4];
};

#define ENTRY(FOURCC, DX_NAME, NAME, FORMAT, TYPE) \
	{\
		.dxFormat= DXGI_FORMAT_##DX_NAME,\
		.fourCC=FOURCC,\
		.glInternalFormat = GL_##NAME,\
		.glFormat= GL_##FORMAT,\
		.glType=GL_##TYPE,\
		.name= #NAME,\
		.swizzle={0x1903, 0x1904, 0x1905, 0x1906}\
	}

#define COMPR(FOURCC, DX_NAME, NAME, FORMAT) \
	{\
		.dxFormat= DXGI_FORMAT_##DX_NAME,\
		.fourCC=FOURCC,\
		.glInternalFormat = GL_##FORMAT,\
		.glFormat= 0,\
		.glType=0,\
		.name= #NAME,\
		.swizzle={0x1903, 0x1904, 0x1905, 0x1906}\
	}

inline const FormatEntry g_FormatTable[] =
{
ENTRY(0, R32G32B32A32_TYPELESS, RGBA, RGBA, UNSIGNED_INT),
ENTRY(TYPE_RGBA32_FLOAT, R32G32B32A32_FLOAT, RGBA32F, RGBA, FLOAT),
ENTRY(0, R32G32B32A32_UINT, RGBA32UI, RGBA_INTEGER, UNSIGNED_INT),
ENTRY(0, R32G32B32A32_SINT, RGBA32I, RGBA_INTEGER, INT),

ENTRY(0, R32G32B32_TYPELESS, RGB, RGB, UNSIGNED_INT),
ENTRY(0, R32G32B32_FLOAT, RGB32F, RGB, FLOAT),
ENTRY(0, R32G32B32_UINT, RGB32UI, RGB_INTEGER, UNSIGNED_INT),
ENTRY(0, R32G32B32_SINT, RGB32I, RGB_INTEGER, INT),

ENTRY(0, R16G16B16A16_TYPELESS, RGBA16, RGBA, UNSIGNED_SHORT),
ENTRY(TYPE_RGBA16_FLOAT, R16G16B16A16_FLOAT, RGBA16F, RGBA, HALF_FLOAT),
ENTRY(TYPE_RGBA16_UNORM, R16G16B16A16_UNORM, RGBA16, RGBA, UNSIGNED_SHORT),
ENTRY(0, R16G16B16A16_UINT, RGBA16UI, RGBA_INTEGER, UNSIGNED_SHORT),
ENTRY(TYPE_RGBA16_SNORM, R16G16B16A16_SNORM, RGBA16_SNORM, RGBA, SHORT),
ENTRY(0, R16G16B16A16_SINT, RGBA16I, RGBA_INTEGER, SHORT),

ENTRY(0, R32G32_TYPELESS, RG, RG, SHORT),
ENTRY(TYPE_RG32_FLOAT, R32G32_FLOAT, RG32F, RG_INTEGER, FLOAT),
ENTRY(0, R32G32_UINT, RG16UI, RG_INTEGER, UNSIGNED_INT),
ENTRY(0, R32G32_SINT, RG16I, RG_INTEGER, INT),

//	ENTRY(0, R32G8X24_TYPELESS,
//	ENTRY(0, D32_FLOAT_S8X24_UINT,
//	ENTRY(0, R32_FLOAT_X8X24_TYPELESS,
//	ENTRY(0, X32_TYPELESS_G8X24_UINT,
ENTRY(0, R10G10B10A2_TYPELESS, RGB10_A2, RGBA, UNSIGNED_INT_10_10_10_2),
ENTRY(0, R10G10B10A2_UNORM, RGB10_A2, RGBA, UNSIGNED_INT_10_10_10_2),
ENTRY(0, R10G10B10A2_UINT, RGB10_A2UI, RGBA, UNSIGNED_INT_10_10_10_2),

//	ENTRY(0, R11G11B10_FLOAT,
ENTRY(0, R8G8B8A8_TYPELESS, RGBA8, RGBA, UNSIGNED_BYTE),
ENTRY(0, R8G8B8A8_UNORM, RGBA8, RGBA, BYTE),
ENTRY(0, R8G8B8A8_UNORM_SRGB, SRGB8_ALPHA8, RGBA, UNSIGNED_BYTE),
ENTRY(0, R8G8B8A8_UINT, RGBA8UI, RGBA, UNSIGNED_BYTE),
ENTRY(0, R8G8B8A8_SNORM, RGBA_SNORM, RGBA, BYTE),
ENTRY(0, R8G8B8A8_SINT, RGBA8I, RGBA, BYTE),

ENTRY(0, R16G16_TYPELESS, RG, RG, UNSIGNED_SHORT),
ENTRY(TYPE_RG16_FLOAT, R16G16_FLOAT, RG16F, RG, HALF_FLOAT),
ENTRY(0, R16G16_UNORM, RG16, RG, UNSIGNED_SHORT),
ENTRY(0, R16G16_UINT, RG16UI, RG, UNSIGNED_SHORT),
ENTRY(0, R16G16_SNORM, RG16_SNORM, RG, SHORT),
ENTRY(0, R16G16_SINT, RG16I, RG, SHORT),

ENTRY(0, R32_TYPELESS, R, R, UNSIGNED_INT),
ENTRY(0, D32_FLOAT, DEPTH_COMPONENT, DEPTH_COMPONENT, FLOAT),
ENTRY(TYPE_R32_FLOAT, R32_FLOAT, R32F, R, FLOAT),
ENTRY(0, R32_UINT, R32UI, R, UNSIGNED_INT),
ENTRY(0, R32_SINT, R32I, R, INT),

//	ENTRY(0, R24G8_TYPELESS,
//	ENTRY(0, D24_UNORM_S8_UINT,
//	ENTRY(0, R24_UNORM_X8_TYPELESS,
//	ENTRY(0, X24_TYPELESS_G8_UINT,
ENTRY(0, R8G8_TYPELESS, RG, RG, UNSIGNED_BYTE),
ENTRY(0, R8G8_UNORM, RG, RG, UNSIGNED_BYTE),
ENTRY(0, R8G8_UINT, RG8UI, RG, UNSIGNED_BYTE),
ENTRY(0, R8G8_SNORM, RG_SNORM, RG, BYTE),
ENTRY(0, R8G8_SINT, RG8I, RG, BYTE),


ENTRY(0, R16_TYPELESS, R16, RED, UNSIGNED_SHORT),
ENTRY(TYPE_R16_FLOAT, R16_FLOAT, R16F, RG, BYTE),
 //
ENTRY(0, D16_UNORM, R16, RED, UNSIGNED_SHORT),
ENTRY(0, R16_UNORM, R16, RED, UNSIGNED_SHORT),
ENTRY(0, R16_UINT, R16UI, RED, UNSIGNED_SHORT),
ENTRY(0, R16_SNORM, R16_SNORM, RED, SHORT),
ENTRY(0, R16_SINT, R16I, RED, SHORT),

ENTRY(0, R8_TYPELESS, R8, RED, UNSIGNED_BYTE),
ENTRY(0, R8_UNORM, R8, RED, UNSIGNED_BYTE),
ENTRY(0, R8_UINT, R8UI, RED, UNSIGNED_BYTE),
ENTRY(0, R8_SNORM, R8_SNORM, RED, BYTE),
ENTRY(0, R8_SINT, R8I, RED, BYTE),

//	ENTRY(0, A8_UNORM,
//	ENTRY(0, R1_UNORM,
//	ENTRY(0, R9G9B9E5_SHAREDEXP,
//	ENTRY(TYPE_RGBG, R8G8_B8G8_UNORM, //
//ENTRY(TYPE_GRGB, G8R8_G8B8_UNORM,
//	ENTRY(TYPE_UVVY, R8G8_B8G8_UNORM, //
//ENTRY(TYPE_YUY2, G8R8_G8B8_UNORM,
//ENTRY(TYPE_ATI2, G8R8_G8B8_UNORM,
//ENTRY(TYPE_CxV8U8, G8R8_G8B8_UNORM,

COMPR(TYPE_DXT1, BC1_TYPELESS, DXT1, COMPRESSED_RGBA_S3TC_DXT1_EXT),
COMPR(TYPE_DXT1, BC1_UNORM, DXT1, COMPRESSED_RGBA_S3TC_DXT1_EXT),
COMPR(TYPE_DXT1, BC1_UNORM_SRGB, DXT1_sRGB, COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT),

COMPR(TYPE_DXT2, BC2_TYPELESS, DXT3, COMPRESSED_RGBA_S3TC_DXT3_EXT),
COMPR(TYPE_DXT3, BC2_UNORM, DXT3, COMPRESSED_RGBA_S3TC_DXT3_EXT),
COMPR(TYPE_DXT3, BC2_UNORM_SRGB, DXT3_sRGB, COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT),

COMPR(TYPE_DXT4, BC3_TYPELESS, DXT5, COMPRESSED_RGBA_S3TC_DXT5_EXT),
COMPR(TYPE_DXT5, BC3_UNORM, DXT5, COMPRESSED_RGBA_S3TC_DXT5_EXT),
COMPR(TYPE_DXT5, BC3_UNORM_SRGB, DXT5_sRGB, COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT),

COMPR(TYPE_BC4_UNORM, BC4_TYPELESS, BC4, COMPRESSED_RED_RGTC1),
COMPR(TYPE_BC4_UNORM, BC4_UNORM, BC4_UNORM, COMPRESSED_RED_RGTC1),
COMPR(TYPE_BC4_SNORM, BC4_SNORM, BC4_SNORM, COMPRESSED_SIGNED_RED_RGTC1),

COMPR(TYPE_BC5_UNORM, BC5_TYPELESS, BC_5, COMPRESSED_RG_RGTC2),
COMPR(TYPE_BC5_UNORM, BC5_UNORM, BC5_UNORM, COMPRESSED_RG_RGTC2),
COMPR(TYPE_BC5_SNORM, BC5_SNORM, BC5_SNORM, COMPRESSED_SIGNED_RG_RGTC2),

ENTRY(0, B5G6R5_UNORM, RGB565, RGB, UNSIGNED_SHORT_5_6_5_REV),
ENTRY(0, B5G5R5A1_UNORM, RGB565, BGR, UNSIGNED_SHORT_5_5_5_1),
ENTRY(0, B8G8R8A8_UNORM, RGBA, BGRA, UNSIGNED_BYTE),
ENTRY(0, B8G8R8X8_UNORM, RGB, BGRA, UNSIGNED_INT_8_8_8_8),
ENTRY(0, R10G10B10_XR_BIAS_A2_UNORM, RGB10_A2, RGBA, UNSIGNED_INT_10_10_10_2),
ENTRY(0, B8G8R8A8_TYPELESS, RGBA, BGRA, UNSIGNED_INT_8_8_8_8),
ENTRY(0, B8G8R8A8_UNORM_SRGB, SRGB_ALPHA, BGRA, UNSIGNED_INT_8_8_8_8),
ENTRY(0, B8G8R8X8_TYPELESS, RGB, BGRA, UNSIGNED_INT_8_8_8_8),
ENTRY(0, B8G8R8X8_UNORM_SRGB, SRGB8, BGRA, UNSIGNED_INT_8_8_8_8),

COMPR(0, BC6H_TYPELESS, BC6, COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB),
COMPR(0, BC6H_UF16, BC6_UF16, COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB),
COMPR(0, BC6H_SF16, BC6_SF16, COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB),

COMPR(0, BC7_TYPELESS,  BC7, COMPRESSED_RGBA_BPTC_UNORM_ARB),
COMPR(0, BC7_UNORM,  BC7, COMPRESSED_RGBA_BPTC_UNORM_ARB),
COMPR(0, BC7_UNORM_SRGB, BC7_sRGB, COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB),

};

enum
{
	NO_ENTIRES = sizeof(g_FormatTable) / sizeof(FormatEntry)
};

#endif // FORMAT_TABLE_HPP