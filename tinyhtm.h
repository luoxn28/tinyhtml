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

	static const char *readName(const char *p, std::string *name);
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

#endif

