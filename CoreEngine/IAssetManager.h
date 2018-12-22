#pragma once
#include "CoreHeaders.h"
#include "IEngineSystem.h"

class IAssetManager : IEngineSystem
{
public:
	virtual CORE_ERROR VRegisterShaders() = 0;
};