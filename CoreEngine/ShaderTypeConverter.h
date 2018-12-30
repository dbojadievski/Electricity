#pragma once
#include "ShaderType.h"
#include "TextureType.h"

CORE_SHADER_TYPE ConvertStringToShaderType (const char * pStr);
CORE_SHADER_INPUT_TYPE ConvertStringToShaderInputType (const char * pStr);

CORE_TEXTURE_TYPE ConvertStringToTextureType (const char * pStr);