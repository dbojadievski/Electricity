#pragma once
#include "CoreTypes.h"
#include "FastDelegate.h"

#include <string>
#include <vector>
#include <queue>
#include <map>
#include <assert.h>

using std::string;

enum EConsoleCommandParameterType
{
	PARAM_DWORD32,
	PARAM_REAL32,
	PARAM_BOOLEAN,
	PARAM_STRING,
	PARAM_TYPE_UNUSED
};

class ConsoleCommandParameterBase
{
	friend class Console;
protected:
	const char * m_pStrName;
	EConsoleCommandParameterType m_Type;
protected:
	ConsoleCommandParameterBase(const char * pStrName, EConsoleCommandParameterType type)
	{
		this->m_pStrName = pStrName;
		this->m_Type = type;
	}
};

typedef ConsoleCommandParameterBase ConsoleCommandParameterDescriptor;

template <class CommandParameterType>
class ConsoleCommandParameter : ConsoleCommandParameterBase
{
	CommandParameterType m_Value;

public:
	ConsoleCommandParameter(const char * pStrName, EConsoleCommandParameterType type, CommandParameterType value) : ConsoleCommandParameterBase(pStrName, type)
	{
		this->m_Value = value;
	}

	ConsoleCommandParameter() { }

	CommandParameterType GetValue()
	{
		return this->m_Value;
	}
};


typedef ConsoleCommandParameterBase CVar;
typedef ConsoleCommandParameter<CORE_DWORD> CVarDWord;
typedef ConsoleCommandParameter<CORE_REAL> CVarReal;
typedef ConsoleCommandParameter<CORE_BOOLEAN> CVarBoolean;
typedef ConsoleCommandParameter<CORE_STRING> CVarString;



