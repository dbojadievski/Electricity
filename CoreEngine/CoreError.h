#pragma once

#include "CoreTypes.h"

enum CORE_ERROR
{
	ERR_OK = 0,
	ERR_SUCCESS = 0,
	ERR_NONE = 0,
	ERR_PARAM_INVALID = 1,
	ERR_PARAM_OUT_OF_RANGE = 2,
	ERR_OBJECT_EXISTS = 3,
	ERR_OBJECT_NOT_FOUND = 4,
	ERR_NOT_IMPLEMENTED = 5,
	ERR_FAILED
};