#pragma once

#include <assert.h>
#include "CoreTypes.h"

class IEngineSystem
{
public:
	virtual void Init() = 0;
	virtual void Update(CORE_DOUBLE dT) = 0;
	virtual void ShutDown() = 0;
};