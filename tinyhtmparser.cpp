#include "tinyhtm.h"

// 定义的转义字符数组
TiHtmBase::Entity TiHtmBase::entity[TiHtmBase::NUM_ENTITY] = 
	{
		{"&amp;", 5, '&'},
		{"&lt;", 4, '<'},
		{"&gt;", 4, '>'},
		{"&quot;", 6, '\"'},
		{"&apos;", 6, '\''}
	};


// the scope of class TiHtmBase

bool TiHtmBase::isWhiteSpace(char c)
{
	return (isspace((unsigned char)c) || c == '\n' || c == '\r');
}

const char *TiHtmBase::skipWhiteSpace(const char *p)
{
	while (p && *p && isWhiteSpace(*p))
	{
		p++;
	}
	return p;
}

const char *TiHtmBase::readName(const char *p, std::string *name)
{
	*name = "";
	assert(p);
	
	if (*p && (isAlpha(*p) || *p == '_'))
	{
		const char *pstart = p;
		while (*p && (isAlpha(*p) || *p == '_' || *p == '-' || *p == '.' || *p == ':'))
		{
			p++;
		}
		if (p - pstart > 0)
		{
			name->assign(pstart, p - pstart);
		}
		return p;
	}
	
	return NULL;
}

const char *TiHtmBase::readText(const char *p, std::string *text, bool trimWhiteSpace, const char *endTag, bool ignoreCase)
{
	*text = "";
	assert(p);
	
	if (!trimWhiteSpace)
	{
		while (*p && !stringEqual(p, endTag, ignoreCase))
		{
			char chr;
			p = getChar(p, &chr);
			text->append(1, chr);
		}
	}
	else
	{
		bool whiteSpace = false;
		
		p = skipWhiteSpace(p);
		while (*p && !stringEqual(p, endTag, ignoreCase))
		{
			if (isWhiteSpace(*p))
			{
				whiteSpace = true;
				p++;
			}
			else
			{
				if (whiteSpace)
				{
					text->append(1, ' ');
					whiteSpace = false;
				}
				
				char chr;
				p = getChar(p, &chr);
				text->append(1, chr);
			}
		}
	}
	
	if (*p)
		p += strlen(endTag);
	
	return (*p) ? p : NULL;
}

const char* TiHtmBase::getEntity(const char *p, char *value)
{
	for (int i = 0; i < NUM_ENTITY; i++)
	{
		if (strncmp(p, entity[i].str, entity[i].strLength) == 0)
		{
			*value = entity[i].chr;
			return (p + entity[i].strLength);
		}
	}
	
	*value = *p;
	return (p + 1);
}

const char *TiHtmBase::getChar(const char *p, char *value)
{
	if (*p == '&')
	{
		return getEntity(p, value);
	}
	else
	{
		*value = *p;
		return (p + 1);
	}
}

/// if tag is p's prefix, return true, else return false
bool TiHtmBase::stringEqual(const char *p, const char *tag, bool ignoreCase)
{
	if (!p || !*p)
	{
		assert(0);
		return false;
	}
	
	const char *pstart = p;
	
	if (ignoreCase)
	{
		while (*pstart && *tag && toLower(*pstart) == toLower(*tag))
		{
			pstart++;
			tag++;
		}
		
		if (!*tag)
			return true;
	}
	else
	{
		while (*pstart && *tag && *pstart == *tag)
		{
			pstart++;
			tag++;
		}
		
		if (!*tag)
			return true;
	}
	return false;
}

int TiHtmBase::isAlpha(unsigned char c)
{
	if (c < 127)
		return isalpha(c);
	else
		return false;
}

int TiHtmBase::isAlnum(unsigned char c)
{
	if (c < 127)
		return isalnum(c);
	else
		return false;
}
