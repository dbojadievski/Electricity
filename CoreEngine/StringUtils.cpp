#include "StringUtils.h"
#include "ArrayUtils.h"
#include <assert.h>
#include <string>
#include <list>

list<string *> * 
tokenize(CORE_STRING pStrCmdText)
{
	assert(pStrCmdText);
		
	list<string *> * pTokens = NULL;
	if (pStrCmdText)
	{
		pTokens = new list<string *>;
		size_t currIdx = -1;
		size_t currTokenStartIdx = -1;
		CORE_CHAR currChar = 0;
		CORE_CHAR currContinuingDelimiter = 0;

		CORE_BOOLEAN isInToken = false;
		CORE_BOOLEAN isDelimitingChar = false;

		CORE_CHAR tokenDelimiters[] = { (CORE_CHAR)' ', (CORE_CHAR)'\t', (CORE_CHAR)'\r', (CORE_CHAR)'\n', (CORE_CHAR)'\r\n', (CORE_CHAR)'"', (CORE_CHAR)'\'' };
		CORE_CHAR tokenContinuingDelimiters[] = { '"', '\'' };

		const size_t numTokenDelimiters = 7;
		const size_t numTokenContinuingDelimiters = 2;

		size_t len = strlen(pStrCmdText);

		for (currIdx = 0; currIdx < len; currIdx++)
		{
			currChar = pStrCmdText[currIdx];
			isDelimitingChar = is_in_array<CORE_CHAR>(tokenDelimiters, currChar, numTokenDelimiters);

			if (isDelimitingChar)
			{
				if (isInToken && (!currContinuingDelimiter || (currContinuingDelimiter == currChar)))
				{
					size_t tokenLength = (currIdx - currTokenStartIdx);
					string * pToken = new string(pStrCmdText, currTokenStartIdx, tokenLength);
					size_t doesntMatter = pTokens->size();
					pTokens->push_back(pToken);

					currContinuingDelimiter = 0;
					isInToken = false;
				}
				else if (!isInToken)
				{
					CORE_CHAR isCurrCharContinuing = is_in_array<CORE_CHAR>(tokenContinuingDelimiters, currChar, numTokenContinuingDelimiters);
					if (isCurrCharContinuing)
						currContinuingDelimiter = currChar;

					currTokenStartIdx = currIdx;
					isInToken = true;
				}
			}
			else
			{
				if (!isInToken)
				{
					currTokenStartIdx = currIdx;
					isInToken = true;
				}
			}
		}

		if (isInToken)
		{
			size_t tokenLength = (currIdx - currTokenStartIdx);
			string * pToken = new string(pStrCmdText, currTokenStartIdx, tokenLength);
			size_t doesntMatter = pTokens->size();
			pTokens->push_back(pToken);

			currContinuingDelimiter = 0;
			isInToken = false;
		}
	}

	return pTokens;
}