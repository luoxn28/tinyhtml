#ifndef TINYHTM_H
#define TINYHTM_H

#include <iostream>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#define TiHtmCout std::cout
#define TiHtmCin  std::cin
#define TiHtmEndl std::endl


/// record the location
struct TiHtmCursor
{
	TiHtmCursor() { clear(); }
	void clear() { row = column = -1; }

	int row;
	int column;
};


/// the base class of whole tinyhtml
class TiHtmBase
{
public:
	TiHtmBase() {}
	virtual ~TiHtmBase() {}

	virtual void print() const = 0;
	virtual const char *parse(const char *p) = 0;

	int row() const { return location.row; }
	int column() const { return location.row; }

	//static void encodeString(const char *str, char *out); // not write code

protected:
	static bool isWhiteSpace(char c);
	static bool isWhiteSpace(int c) 
	{ 
		if (c < 256)
			return isWhiteSpace((char)c);
		return false;
	}
	static const char *skipWhiteSpace(const char *p);

	/** Read a html name into the string provided. Return a pointer
	    pointer just past the last character fo the name.
	    return NULL if has error.
	*/
	static const char *readName(const char *p, std::string *name);
	/// 读取element中的text，直到遇到endTag为止
	static const char *readText(const char *p, std::string *text, bool trimWhiteSpace, const char *endTag, bool ignoreCase);
	/// 把转义字符转化为原来的字符，比如"&lt;"转化为 '<'
	static const char *getEntity(const char *p, char *value);
	/// 从输入流中获取一个字符，有可能转换转义字符
	static const char *getChar(const char *p, char *value);
	
	/// if tge is p's prefix, return true, else return false
	static bool stringEqual(const char *p, const char *tag, bool ignoreCase);
	
	// record the location message
	TiHtmCursor location;
	
	static int isAlpha(unsigned char c);
	static int isAlnum(unsigned char c);
	static int toLower(int c) { return tolower(c); }

private:
	TiHtmBase(const TiHtmBase &);			// not allowd
	void operator=(const TiHtmBase &base); // not allowd

	struct Entity
	{
		const char *str;
		unsigned int strLength;
		const char chr;
	};
	enum
	{
		NUM_ENTITY = 5,
		MAX_ENTITY_LENGTH = 6
	};

	static Entity entity[NUM_ENTITY]; // defined in file tinyhtmparser.cpp
};


class TiHtmDocument
{
public:
	TiHtmDocument() : value("") {}
	TiHtmDocument(std::string filename) : value(filename) {}

	bool loadFile();
	bool loadFile(const char *filename);
	bool loadFile(const std::string filename);
	bool loadFile(FILE *file);

	const std::string getValue() const { return value; } 
	const char *getValueCstr()const  { return value.c_str(); }
	
private:
	// This should not in here, it will in TiHtmNode
	std::string value;
};

class TiHtmNode : public TiHtmBase
{
public:

	/// the type of html nodes
	enum NodeType
	{
		TINYHTM_DOCUMENT,
		TINYHTM_ELEMENT,
		TINYHTM_COMMENT,
		TINYHTM_UNKNOWN,
		TINYHTM_TEXT,
		TINYHTM_DECLARATION,
		TINYHTM_TYPTCOUNT
	};
	
	virtual ~TiHtmNode();
	
	/**
		the different meaning of value
		
		Document:	filename of the xml file
		Element:	name of the element
		Comment:	the comment text
		Unknown:	the tag contents
		Text:		the text string
	*/
	const std::string getValue() const { return value; }
	const char  *getValueStr() const { return value.c_str(); }
	void setValue(const char *_value) { value = _value; }
	void setValue(const std::string &_value) { value = _value; }
	
	/// delete alll the child nodes
	void clear();
	
	TiHtmNode *getParent() 					{ return parent; }
	const TiHtmNode *getParent() const 	{ return parent; }
	TiHtmNode *getFirstChild()				{ return firstChild; }
	const TiHtmNode *getFirstChild() const { return firstChild; }
	TiHtmNode *getLastChild() 				{ return lastChild; }
	const TiHtmNode *getLastChild() const  { return lastChild; }
	
	const TiHtmNode *getFirstChild(const char *_value) const;
	TiHtmNode *getFirstChild(const char *_value)
	{
		return const_cast<TiHtmNode *>((const_cast<const TiHtmNode*>(this))->getFirstChild(_value));
	}
	const TiHtmNode *getFirstChild(const std::string &_value) const { return getFirstChild(_value.c_str()); }
	TiHtmNode *getFirstChild(const std::string &_value) { return getFirstChild(_value.c_str()); }
	
	const TiHtmNode *getLastChild(const char *_value) const;
	TiHtmNode *getLastChild(const char *_value)
	{
		return const_cast<TiHtmNode *>((const_cast<const TiHtmNode*>(this))->getLastChild(_value));
	}
	const TiHtmNode *getLastChild(const std::string &_value) const { return getLastChild(_value.c_str()); }
	TiHtmNode *geLastChild(const std::string &_value) { return getLastChild(_value.c_str()); }
	
	TiHtmNode *insertEndChild(const TiHtmNode &addNode);
	TiHtmNode *linkEndChild(TiHtmNode *pnode);
	
	/** the type of this node
	    they are TINYHTM_DOCUMENT,TINYHTM_ELEMENT,TINYHTM_COMMENT,
		TINYHTM_UNKNOWN,TINYHTM_TEXT,TINYHTM_DECLARATION
	*/
	int getType() const { return type; }
	
	virtual TiHtmNode *clone() const = 0;
	
protected:
	TiHtmNode(NodeType _type);
	
	TiHtmNode *parent;
	NodeType type;
	TiHtmNode *firstChild;
	TiHtmNode *lastChild;
	
	std::string value;
	
	TiHtmNode *prev;
	TiHtmNode *next;
};

#endif

