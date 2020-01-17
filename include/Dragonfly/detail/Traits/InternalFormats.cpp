#include "InternalFormats.h"

GLuint df::detail::getInternalFormat(
		int num_of_channels,
		int bytes_per_channel)
{
	// todo: can be optimized, for example
	// num_of_channels << 8 + bytes_per_channel
	// then use only one switch. Extra error checking is needed in DEBUG
	// mode to avoid loosing the different error messages.
	GLint i = 0;
	switch(num_of_channels)
	{
	case 1: // GL_RED;
		switch(bytes_per_channel)
		{
		case 8:  i = GL_R8; break;
		case 16: i = GL_R16F; break;
		case 32: i = GL_R32F; break;
		default: ASSERT(false, "Invalid number of bytes for a one channel image."); break;
		}
		break;
	case 2: // GL_RG;
		switch(bytes_per_channel)
		{
		case 8:  i = GL_RG8; break;
		case 16: i = GL_RG16F; break;
		case 32: i = GL_RG32F; break;
		default: ASSERT(false, "Invalid number of bytes for a two channel image."); break;
		}
		break;
	case 3: // GL_RGB;
		switch(bytes_per_channel)
		{
		case 3:  i = GL_R3_G3_B2;
			WARNING(true, "Third channel is two bit in length, not three.");
			break;
		case 4:  i = GL_RGB4; break;
		case 5:  i = GL_RGB5; break;
		case 8:  i = GL_RGB8; break;
		case 9:  i = GL_RGB9_E5;
			WARNING(true, "The exponent (5 bits) are shared");
			break;
		case 10: i = GL_RGB10; break;
		case 11: i = GL_R11F_G11F_B10F;
			WARNING(true, "Third channel is ten bit in length, not eleven.");
			break;
		case 12: i = GL_RGB12; break;
		case 16: i = GL_RGB16F; break;
		case 32: i = GL_RGB32F; break;
		default: ASSERT(false, "Invalid number of bytes for a three channel image."); break;
		}
		break;
	case 4: // GL_RGBA;
		switch(bytes_per_channel)
		{
		case 3:  i = GL_R3_G3_B2; break;
		case 4:  i = GL_RGBA4; break;
		case 5:  i = GL_RGB5_A1;
			WARNING(true, "Alpha channel is one bit in length, not five.");
			break;
		case 8:  i = GL_RGBA8; break;
		case 10: i = GL_RGB10_A2;
			WARNING(true, "Alpha channel is two bit in length, not ten.");
			break;
		case 12: i = GL_RGBA12; break;
		case 16: i = GL_RGBA16F; break;
		case 32: i = GL_RGBA32F; break;
		default: ASSERT(false, "Invalid number of bytes for a four channel image."); break;
		}
		break;
	default: ASSERT(false, "Invalid number of channels"); break;
	}
	return i;
}

GLuint df::detail::getBaseFormat(GLuint internalFormat)
{
	switch(internalFormat)
	{
	case GL_DEPTH_COMPONENT16:	case GL_DEPTH_COMPONENT24:		case GL_DEPTH_COMPONENT32:
	case GL_DEPTH_COMPONENT32F:
		return GL_DEPTH_COMPONENT;
	case GL_STENCIL_INDEX1:		case GL_STENCIL_INDEX4:			case GL_STENCIL_INDEX8:
	case GL_STENCIL_INDEX16:
		return GL_STENCIL_INDEX;
	case GL_DEPTH24_STENCIL8:	case GL_DEPTH32F_STENCIL8:
		return GL_DEPTH_STENCIL;
	case GL_R8:					case GL_R8_SNORM:				case GL_R16:
	case GL_R16_SNORM:			case GL_R16F:					case GL_R32F:
	case GL_R8I:				case GL_R8UI:					case GL_R16I:
	case GL_R16UI:				case GL_R32I:					case GL_R32UI:
	case GL_COMPRESSED_RED:		case GL_COMPRESSED_RED_RGTC1:	case GL_COMPRESSED_SIGNED_RED_RGTC1:
		return GL_RED;
	case GL_RG8:				case GL_RG8_SNORM:				case GL_RG16:
	case GL_RG16_SNORM:			case GL_RG16F:					case GL_RG32F:
	case GL_SRGB8:				case GL_R11F_G11F_B10F:			case GL_RGB9_E5:
	case GL_RG8I:				case GL_RG8UI:					case GL_RG16I:
	case GL_RG16UI:				case GL_RG32I:					case GL_RG32UI:
	case GL_COMPRESSED_RG:		case GL_COMPRESSED_RG_RGTC2:	case GL_COMPRESSED_SIGNED_RG_RGTC2:
		return GL_RG;
	case GL_R3_G3_B2:			case GL_RGB4:					case GL_RGB5:
	case GL_RGB8:				case GL_RGB8_SNORM:				case GL_RGB10:
	case GL_RGB12:				case GL_RGB16:
	case GL_RGB16_SNORM:		case GL_RGB16F:
	case GL_RGB32F:				case GL_RGB8I:					case GL_RGB8UI:
	case GL_RGB16I:				case GL_RGB16UI:				case GL_RGB32I:
	case GL_RGB32UI:			case GL_COMPRESSED_RGB:			case GL_COMPRESSED_SRGB:
	case GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT:	case GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT:
		return GL_RGB;
	case GL_RGBA2:				case GL_RGBA4:					case GL_RGB5_A1:
	case GL_RGBA8:				case GL_RGBA8_SNORM:			case GL_RGB10_A2:
	case GL_RGB10_A2UI:			case GL_RGBA12:					case GL_RGBA16:
	case GL_SRGB8_ALPHA8:		case GL_RGBA16F:				case GL_RGBA32F:
	case GL_RGBA8I:				case GL_RGBA8UI:				case GL_RGBA16I:
	case GL_RGBA16UI:			case GL_RGBA32I:				case GL_RGBA32UI:
		return GL_RGBA;
	default:
		ASSERT(false, "Invalid internal format!");
		return GL_INVALID_ENUM;
	}
}

/*GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, GL_INT, GL_FLOAT,
GL_UNSIGNED_BYTE_3_3_2, GL_UNSIGNED_BYTE_2_3_3_REV, GL_UNSIGNED_SHORT_5_6_5,
GL_UNSIGNED_SHORT_5_6_5_REV, GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_4_4_4_4_REV,
GL_UNSIGNED_SHORT_5_5_5_1, GL_UNSIGNED_SHORT_1_5_5_5_REV, GL_UNSIGNED_INT_8_8_8_8,
GL_UNSIGNED_INT_8_8_8_8_REV, GL_UNSIGNED_INT_10_10_10_2, GL_UNSIGNED_INT_2_10_10_10_REV.*/

GLuint df::detail::getBaseType(GLuint internalFormat)
{
	switch(internalFormat)
	{
	// Trivial cases
	case GL_R8:					case GL_RG8:					case GL_RGB8:
	case GL_RGBA8:				case GL_R8UI:					case GL_RG8UI:
	case GL_RGB8UI:				case GL_RGBA8UI:
		return GL_UNSIGNED_BYTE;
	case GL_R8_SNORM:			case GL_RG8_SNORM:				case GL_RGB8_SNORM:
	case GL_RGBA8_SNORM:		case GL_R8I:					case GL_RG8I:
	case GL_RGB8I:				case GL_RGBA8I:
		return GL_BYTE;
	case GL_R16:				case GL_RG16:					case GL_RGB16:
	case GL_RGBA16:				case GL_R16UI:					case GL_RG16UI:
	case GL_RGB16UI:			case GL_RGBA16UI:
		return GL_UNSIGNED_SHORT;
	case GL_R16_SNORM:			case GL_RG16_SNORM:				case GL_RGB16_SNORM:
	case GL_RGBA16_SNORM:		case GL_R16I:					case GL_RG16I:
	case GL_RGB16I:				case GL_RGBA16I:
		return GL_SHORT;
	case GL_R32UI:				case GL_RG32UI:					case GL_RGB32UI:
	case GL_RGBA32UI:
		return GL_UNSIGNED_INT;
	case GL_R32I:				case GL_RG32I:					case GL_RGB32I:
	case GL_RGBA32I:
		return GL_INT;
	case GL_R16F:				case GL_RG16F:					case GL_RGB16F:
	case GL_RGBA16F:			case GL_R32F:					case GL_RG32F:
	case GL_RGB32F:				case GL_RGBA32F:
		return GL_FLOAT;
	// Depth and Stencil
	case GL_DEPTH_COMPONENT16:	case GL_DEPTH_COMPONENT24:		case GL_DEPTH_COMPONENT32:
		WARNING(true, "Downcast will occur.");
	case GL_DEPTH_COMPONENT32F:
		return GL_FLOAT;
	case GL_STENCIL_INDEX4:			case GL_STENCIL_INDEX1:
		WARNING(true, "Downcast will occur.");
	case GL_STENCIL_INDEX8:
		return GL_BYTE;
	case GL_DEPTH24_STENCIL8:		case GL_DEPTH32F_STENCIL8:
		ASSERT(false, "No default upload method for these types");
		return GL_INVALID_ENUM;
	//Not so trivial cases
	case GL_R3_G3_B2:
		return GL_UNSIGNED_BYTE_3_3_2;
	case GL_RGB5_A1:
		return GL_UNSIGNED_SHORT_5_5_5_1;
	case GL_RGB10_A2:			case GL_RGB10_A2UI:
		return GL_UNSIGNED_INT_10_10_10_2;
	case GL_RGB10:				case GL_RGB12:					case GL_RGBA12:
		WARNING(true, "Downcast will occur.");
		return GL_UNSIGNED_INT;
	case GL_RGB4:				case GL_RGB5:					case GL_RGBA2:
	case GL_RGBA4:
		WARNING(true, "Downcast will occur.");
	case GL_SRGB8:					case GL_SRGB8_ALPHA8:
		return GL_BYTE;
	case GL_R11F_G11F_B10F:		case GL_RGB9_E5:
		return GL_FLOAT;
	default:
		ASSERT(false, "Invalid internal format!");
		return GL_INVALID_ENUM;
	}
}
