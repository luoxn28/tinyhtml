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


// the scope of class TiHtmParsingData

class TiHtmParsingData
{
	friend class TiHtmDocument;
public:
	void setStamp(const char *p);
	
	const TiHtmCursor &getCursor() const { return cursor; }

private:
	TiHtmParsingData(const char *pstart, int _tabsize, int row, int column)
	{
		assert(pstart);
		stamp = pstart;
		tabsize = _tabsize;
		cursor.row = row;
		cursor.column = column;
	}
	
	TiHtmCursor cursor;
	const char *stamp;
	int 		 tabsize; 
};

/// 在parse时更新cursor值，比如换行时row++，col=0否则row++,col++,最后会把stamp复制为now
void TiHtmParsingData::setStamp(const char *now)
{
	assert(now);
	if (tabsize < 1)
		return;
	
	// Get the current now, column
	int row = cursor.row;
	int col = cursor.column;
	const char *p = stamp;
	assert(p);
	
	while (p < now)
	{
		const unsigned char *pU = (const unsigned char *)p;
		
		switch (*pU)
		{
			case 0:
				return;
			
			case '\r':
				row++;
				col = 0;
				// Eat the character
				p++;
				
				// Check for \r\n
				if (*p == '\n')
				{
					p++;
				}
				break;
			
			case '\n':
				row++;
				col = 0;
				// Eat the character
				p++;
				
				// Check for \r\n
				if (*p == '\r')
				{
					p++;
				}
				break;
			
			case '\t':
				// Eat the character
				p++;
				// Skip to next tab stop
				col = (col / tabsize + 1) * tabsize;
				break;
			
			default:
				p++;
				col++;
				break;
		}
	}
	
	cursor.row = row;
	cursor.column = col;
	assert(cursor.row >= -1);
	assert(cursor.column >= -1);
	stamp = p;
	assert(stamp);
}

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

// the scope of class TiHtmNode

/// 解析输入流中字符是什么，然后new相应的类型并返回
TiHtmNode *TiHtmNode::identify(const char *p)
{
	TiHtmNode *returnNode = NULL;
	
	p = skipWhiteSpace(p);
	if (!p || !*p || *p != '<')
		return NULL;
	
	p = skipWhiteSpace(p);
	if (!p || !*p)
		return NULL;
	
	//const char *htmlHeader = "<html";
	const char *commentHeader = "<!--";
	const char *dtdHeader = "<!";
	
	/*if (stringEqual(p, htmlHeader, true))
	{
		//returnNode = new TiHtmDeclaration();
	}
	else */
	if (stringEqual(p, commentHeader, true))
	{
		//returnNode = new TiHtmComment();
	}
	else if (stringEqual(p, dtdHeader, true))
	{
		//returnNode = new TiHtmUnknown();
	}
	else if (isAlpha(*(p+1)) || *(p+1) == '_')
	{
		returnNode = new TiHtmElement("");
	}
	else
	{
		//returnNode = new TiHtmElement();
	}
	
	if (returnNode)
	{
		returnNode->parent = this;
	}
	
	return returnNode;
}

// the scope of class TiHtmElement

/// 真正开始parse的第一个字符必须是'<'
const char *TiHtmElement::parse(const char *p, TiHtmParsingData *data)
{	
	if (!p || !*p)
	{ 
		std::cout << "TiHtmNode::parse()(1) error" << std::endl;
		return NULL;
	}
	
	p = skipWhiteSpace(p);
	
	if (data)
	{
		data->setStamp(p);
		location = data->getCursor();
	}
	
	if (*p != '<')
	{
		std::cout << "TiHtmNode::parse()(2) error" << std::endl;
		return NULL;
	}
	
	p = skipWhiteSpace(p + 1);
	
	p = readName(p, &value);
	if (!p || !*p)
	{
		std::cout << "TiHtmNode::parse()(3) error" << std::endl;
		return NULL;
	}
	
	std::string endTag = "</";
	endTag += value;
	
	while (p && *p)
	{
		p = skipWhiteSpace(p);
		if (!p || !*p)
		{
			std::cout << "TiHtmNode::parse()(4) error" << std::endl;
			return NULL;
		}
		
		if (*p == '/')
		{
			p++;
			if (*p != '>')
			{
				std::cout << "TiHtmNode::parse()(5) error" << std::endl;
				return (p + 1);
			}
		}
		else if (*p == '>')
		{
			// Done with attributes (if there were any.)
			// Read the value -- which can include other
			// elements -- read the end tag, and return.
			p++;
			p = readValue(p, data);
			if (!p || !*p)
			{
				std::cout << "TiHtmNode::parse()(6) error" << std::endl;
				return NULL;
			}
			
			if (stringEqual(p, endTag.c_str(), false))
			{
				p += endTag.length();
				p = skipWhiteSpace(p);
				if (p && *p && *p == '>')
				{
					p++;
					return p;
				}
				
				std::cout << "TiHtmNode::parse()(7) error" << std::endl;
				return NULL;
			}
			else
			{
				std::cout << "TiHtmNode::parse()(8) error" << std::endl;
				return NULL;
			}
		}
		else
		{
			p++;
			if (!p || !*p)
			{
				std::cout << "TiHtmNode::parse()(9) error" << std::endl;
				return NULL;
			}
		}
	}
	
	return p;
}

const char *TiHtmElement::readValue(const char *p, TiHtmParsingData *data)
{
	const char *pWithWhiteSpace = p;
	
	p = skipWhiteSpace(p);
	while (p && *p)
	{
		if (*p != '<')
		{
			// It is text element
			TiHtmText *textNode = new TiHtmText("");
			if (!textNode)
				return NULL;
			
			p = textNode->parse(p, data);
		
			if (!textNode->blank())
				linkEndChild(textNode);
			else
				delete textNode;
		}
		else
		{
			// We hit a '<'
			// Have we hit a new element or an end tag?
			if (stringEqual(p, "</", false))
			{
				return p;
			}
			else
			{
				// other element
				TiHtmNode *node = identify(p);
				if (node)
				{
					p = node->parse(p, data);
					linkEndChild(node);
				}
				else
				{
					return NULL;
				}
			}
		}
		
		pWithWhiteSpace = p;
		p = skipWhiteSpace(p);
	}
}

// the scope of class TiHtmText

const char *TiHtmText::parse(const char *p, TiHtmParsingData *data)
{
	value = "";
	
	if (data)
	{
		data->setStamp(p);
		location = data->getCursor();
	}
	
	bool ignoreWhite = true;
	
	const char *end = "<";
	p = readText(p, &value, ignoreWhite, end, false);
	if (p && *p)
		return p - 1;
	
	return NULL;
}

bool TiHtmText::blank() const
{
	for (unsigned int i = 0; i < value.length(); i++)
	{
		if (!isWhiteSpace(value[i]))
			return false;
	}
	
	return true;
}

// the scope of class TiHtmDocument

const char *TiHtmDocument::parse(const char *p, TiHtmParsingData *prevData)
{
	if (!p || !*p)
	{
		std::cout << "TiHtmDocument::parse()(1) error" << std::endl;
		return NULL;
	}
	
	location.clear();
	if (prevData)
	{
		location.row = prevData->cursor.row;
		location.column = prevData->cursor.column;
	}
	else
	{
		location.row = 0;
		location.column = 0;
	}
	
	TiHtmParsingData data(p, tabSize(), location.row, location.column);
	location = data.getCursor();
	
	p = skipWhiteSpace(p);
	if (!p || !*p)
	{
		std::cout << "TiHtmDocument::parse()(2) error" << std::endl;
		return NULL;
	}
	
	while (p && *p)
	{
		TiHtmNode *pnode = identify(p);
		
		if (pnode)
		{
			p = pnode->parse(p, &data);
			linkEndChild(pnode);
		}
		else
		{
			break;
		}
		
		p = skipWhiteSpace(p);
	}
	
	if (!firstChild)
	{
		std::cout << "TiHtmDocument::parse()(3) error" << std::endl;
		return NULL;
	}
	
	// All is OK
	return p;
}







