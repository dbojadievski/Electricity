#include "ShaderTypeConverter.h"
#include "ShaderType.h"
#include "TextureType.h"

#include <string>
#include <algorithm>

using std::string;

CORE_SHADER_TYPE
ConvertStringToShaderType (const char * pStr)
{
    CORE_SHADER_TYPE shaderType = SHADER_TYPE_UNUSED;

    string asString (pStr);
    transform (asString.begin (), asString.end (), asString.begin (), ::tolower);

    if (!strcmp (pStr, "vertex"))
        shaderType = SHADER_TYPE_VERTEX;
    else if (!strcmp (pStr, "pixel"))
        shaderType = SHADER_TYPE_PIXEL;
    else if (!strcmp (pStr, "compute"))
        shaderType = SHADER_TYPE_COMPUTE;

    return shaderType;
}

CORE_SHADER_INPUT_TYPE
ConvertStringToShaderInputType (const char * pStr)
{
    CORE_SHADER_INPUT_TYPE shaderInputType = SHADER_INPUT_UNUSED;

    string asString (pStr);
    transform (asString.begin (), asString.end (), asString.begin (), ::tolower);

    if (!strcmp (pStr, "vertex"))
        shaderInputType     = SHADER_INPUT_PER_VERTEX;
    else if (!strcmp (pStr, "instance"))
        shaderInputType     = SHADER_INPUT_PER_INSTANCE;

    return shaderInputType;
}

CORE_TEXTURE_TYPE
ConvertStringToTextureType (const char * pStr)
{
    CORE_TEXTURE_TYPE type = TEXTURE_TYPE_UNUSED;

    string asString (pStr);
    transform (asString.begin (), asString.end (), asString.begin (), ::tolower);

    if (!strcmp (pStr, "1d"))
        type = TEXTURE_TYPE_1D;
    else if (!strcmp (pStr, "2d"))
        type = TEXTURE_TYPE_2D;
    else if (!strcmp (pStr, "3d"))
        type = TEXTURE_TYPE_3D;

    return type;
}