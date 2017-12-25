#pragma once

#include "CoreTypes.h"

class ShaderBase
{
protected:
	virtual CORE_BOOLEAN Compile() = 0;
public:
	ShaderBase(CORE_STRING pStrVertexShader, CORE_STRING pStrFragmentShader) 
	{
	}
};

