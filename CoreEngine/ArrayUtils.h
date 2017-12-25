#pragma once
#include "CoreTypes.h"
#include <assert.h>


template<typename Type>
CORE_BOOLEAN is_in_array(Type haystack[], Type needle, size_t haystackSize)
{
	CORE_BOOLEAN needleFound = false;

	for (size_t currIdx = 0; currIdx < haystackSize; currIdx++)
		if (needle == haystack[currIdx])
		{
			needleFound = true;
			break;
		}

	return needleFound;
}