#pragma once
#include "CoreTypes.h"

#include <list>
#include <string>

using std::string;
using std::list;

list<string *> * tokenize(CORE_STRING pStrCmdText);