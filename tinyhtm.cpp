#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "tinyhtm.h"

const char *TIHTMVERSION = "intyhtml-0.1";

const char *TiHtmVersion()
{
	return TIHTMVERSION;
}
static FILE *TiHtmOpen(const char *filename, const char *mode)
{
	return fopen(filename, mode);
}

// the scope of class TiHtmBase

void TiHtmBase::encodeString(const std::string& str, std::string* outString)
{
	int i = 0, len = str.length();
	
	while (i < len)
	{
		unsigned char c = (unsigned char)str[i];
		
		if (c == '&' && (i < len - 2) && (str[i+1] == '#') && (str[i+2] == 'x'))
		{
			// Hexadecimal character reference.
			// Pass through unchanged.
			// &#xA9;	-- copyright symbol, for example.
			//
			// The -1 is a bug fix from Rob Laveaux. It keeps
			// an overflow from happening if there is no ';'.
			// There are actually 2 ways to exit this loop -
			// while fails (error case) and break (semicolon found).
			// However, there is no mechanism (currently) for
			// this function to return an error.
			while (i < len - 1)
			{
				outString->append(str.c_str() + i, 1);
				++i;
				if ( str[i] == ';' )
					break;
			}
		}
		else if (c == '&')
		{
			outString->append(entity[0].str, entity[0].strLength);
			++i;
		}
		else if ( c == '<' )
		{
			outString->append(entity[1].str, entity[1].strLength);
			++i;
		}
		else if ( c == '>' )
		{
			outString->append(entity[2].str, entity[2].strLength);
			++i;
		}
		else if ( c == '\"' )
		{
			outString->append(entity[3].str, entity[3].strLength);
			++i;
		}
		else if ( c == '\'' )
		{
			outString->append(entity[4].str, entity[4].strLength);
			++i;
		}
		// 特殊不显示的字符
		else if (c < 32)
		{
			// Easy pass at non-alpha/numeric/symbol
			// Below 32 is symbolic.
			char buf[ 32 ];
			
			snprintf(buf, sizeof(buf), "&#x%02X;", (unsigned)( c & 0xff ));

			//*ME:	warning C4267: convert 'size_t' to 'int'
			//*ME:	Int-Cast to make compiler happy ...
			outString->append( buf, (int)strlen( buf ) );
			++i;
		}
		else
		{
			*outString += (char)c;
			++i;
		}
	}
}

// the scope of class TiHtmNode

TiHtmNode::TiHtmNode(NodeType _type)
{
	parent = NULL;
	firstChild = lastChild = NULL;
	value = _type;
	
	prev = next = NULL;
}

TiHtmNode::~TiHtmNode()
{
	TiHtmNode *pnode = firstChild;
	TiHtmNode *ptmp = NULL;
	
	while (pnode)
	{
		ptmp = pnode;
		pnode = pnode->next;
		delete ptmp;
	}
}

void TiHtmNode::clear()
{
	TiHtmNode *pnode = firstChild;
	TiHtmNode *ptmp = NULL;
	
	while (pnode)
	{
		ptmp = pnode;
		pnode = pnode->next;
		delete ptmp;
	}
	firstChild = lastChild = NULL;
}

const TiHtmNode *TiHtmNode::getFirstChild(const char *_value) const
{
	const TiHtmNode *pnode = NULL;
	
	for (pnode = firstChild; pnode; pnode = pnode->next)
	{
		if (strcmp(pnode->getValueStr(), _value) == 0)
		{
			return pnode;
		}
	}
	return NULL;
}

const TiHtmNode *TiHtmNode::getLastChild(const char *_value) const
{
	const TiHtmNode *pnode = NULL;
	
	for (pnode = lastChild; pnode; pnode = pnode->prev)
	{
		if (strcmp(pnode->getValueStr(), _value) == 0)
		{
			return pnode;
		}
	}
	return NULL;
}

TiHtmNode *TiHtmNode::insertEndChild(const TiHtmNode &addNode)
{
	if (addNode.getType() == TiHtmNode::TINYHTM_DOCUMENT)
	{
		std::cout << "TiHtmNode::insertEndChild() error" << std::endl;
		return NULL;
	}
	
	TiHtmNode *pnode = addNode.clone();
	if (!pnode)
		return NULL;
	
	return linkEndChild(pnode);
}

TiHtmNode *TiHtmNode::linkEndChild(TiHtmNode *pnode)
{
	/*
	if (pnode->getType() == TiHtmNode::TINYHTM_DOCUMENT)
	{
		delete pnode;
		std::cout << "TiHtmNode::linkEndChild() error" << std::endl;
		return NULL;
	}
	*/
	pnode->parent = this;
	pnode->prev = lastChild;
	pnode->next = NULL;
	
	if (lastChild)
		lastChild->next = pnode;
	else
		firstChild = pnode;
	
	lastChild = pnode;
	return pnode;
}

TiHtmNode *TiHtmNode::insertBeforeChild(TiHtmNode *beforeNode, const TiHtmNode &addNode)
{
	if (!beforeNode || beforeNode->parent != this)
	{
		return NULL;
	}
	if (addNode.getType() == TiHtmNode::TINYHTM_DOCUMENT)
	{
		std::cout << "TiHtmNode::insertBeforeChild() error" << std::endl;
		return NULL;
	}
	
	TiHtmNode *pnode = addNode.clone();
	if (!pnode)
		return NULL;
	pnode->parent = this;
	
	pnode->next = beforeNode;
	pnode->prev = beforeNode->prev;
	if (beforeNode->prev)
	{
		beforeNode->prev->next = pnode;
	}
	else
	{
		assert(firstChild == beforeNode);
		firstChild = pnode;
	}
	
	beforeNode->prev = pnode;
	return pnode;
}

TiHtmNode *TiHtmNode::insertAfterChild(TiHtmNode *afterNode, const TiHtmNode &addNode)
{
	if (!afterNode || afterNode->parent != this)
	{
		return NULL;
	}
	if (addNode.getType() == TiHtmNode::TINYHTM_DOCUMENT)
	{
		std::cout << "TiHtmNode::insertAfterChild() error" << std::endl;
		return NULL;
	}
	
	TiHtmNode *pnode = addNode.clone();
	if (!pnode)
		return NULL;
	pnode->parent = this;
	
	pnode->prev = afterNode;
	pnode->next = afterNode->next;
	if (afterNode->next)
	{
		afterNode->next->prev = pnode;
	}
	else
	{
		assert(lastChild == afterNode);
		lastChild = pnode;
	}
	
	afterNode->next = pnode;
	return pnode;
}

TiHtmNode *TiHtmNode::replaceChild(TiHtmNode *replaceNode, const TiHtmNode &withNode)
{
	if (!replaceNode || replaceNode->parent != this)
	{
		return NULL;
	}
	if (withNode.getType() == TiHtmNode::TINYHTM_DOCUMENT)
	{
		std::cout << "TiHtmNode::replaceChild() error" << std::endl;
		return NULL;
	}
	
	TiHtmNode *pnode = withNode.clone();
	if (!pnode)
		return NULL;
	
	pnode->prev = replaceNode->prev;
	pnode->next = replaceNode->next;
	
	if (replaceNode->prev)
	{
		replaceNode->prev->next = pnode;
	}
	else
	{
		assert(replaceNode == firstChild);
		firstChild = pnode;
	}
	
	if (replaceNode->next)
	{
		replaceNode->next->prev = pnode;
	}
	else
	{
		assert(replaceNode == lastChild);
		lastChild = pnode;
	}
	
	delete replaceNode;
	pnode->parent = this;
	return pnode;
}

bool TiHtmNode::removeChild(TiHtmNode *removeNode)
{
	if (!removeNode || removeNode->parent != this)
	{
		return false;
	}
	
	if (removeNode->prev)
	{
		removeNode->prev->next = removeNode->next;
	}
	else
	{
		assert(firstChild == removeNode);
		firstChild = removeNode->next;
	}
	
	if (removeNode->next)
	{
		removeNode->next->prev = removeNode->prev;
	}
	else
	{
		assert(lastChild == removeNode);
		lastChild = removeNode->prev;
	}
	
	delete removeNode;
	return true;
}

const TiHtmNode *TiHtmNode::previousSibling(const char *_value) const
{
	const TiHtmNode *pnode = NULL;
	for (pnode = prev; pnode; pnode = pnode->prev)
	{
		if (strcmp(pnode->getValueStr(), _value) == 0)
			return pnode;
	}
	return NULL;
}

const TiHtmNode *TiHtmNode::nextSibling(const char *_value) const
{
	const TiHtmNode *pnode = NULL;
	for (pnode = next; pnode; pnode = pnode->next)
	{
		if (strcmp(pnode->getValueStr(), _value) == 0)
			return pnode;
	}
	return NULL;
}

const TiHtmElement *TiHtmNode::nextSiblingElement() const
{
	const TiHtmNode *pnode = NULL;
	for (pnode = next; pnode; pnode = pnode->next)
	{
		if (pnode->toElement())
			return pnode->toElement();
	}
	return NULL;
}

const TiHtmElement *TiHtmNode::nextSiblingElement(const char *_value) const
{
	const TiHtmNode *pnode = NULL;
	for (pnode = nextSibling(_value); pnode; pnode = pnode->nextSibling(_value))
	{
		if (pnode->toElement())
			return pnode->toElement();
	}
	return NULL;
}

const TiHtmElement *TiHtmNode::firstChildElement() const
{
	const TiHtmNode *pnode = NULL;
	
	for (pnode = getFirstChild(); pnode; pnode = pnode->nextSibling())
	{
		if (pnode->toElement())
			return pnode->toElement();
	}
	return NULL;
}

const TiHtmElement *TiHtmNode::firstChildElement(const char *_value) const
{
	const TiHtmNode *pnode = NULL;
	
	for (pnode = getFirstChild(_value); pnode; pnode = pnode->nextSibling(_value))
	{
		if (pnode->toElement())
			return pnode->toElement();
	}
	return NULL;
}

void TiHtmNode::copyTo(TiHtmNode *target) const
{
	target->setValue(value.c_str());
	target->location = location;
}
// the scope of class TiHtmNode (end)

// the scope of class TiHtmAttribute (start)

int TiHtmAttribute::intValue() const
{
	return atoi(value.c_str());
}

double TiHtmAttribute::doubleValue() const
{
	return atof(value.c_str());
}

int TiHtmAttribute::queryIntValue(int* ival) const
{
	if (sscanf(value.c_str(), "%d", ival) == 1)
		return TIHTM_SUCCESS;
	return TIHTM_WRONG_TYPE;
}

int TiHtmAttribute::queryDoubleValue(double* dval) const{
	if (sscanf(value.c_str(), "%lf", dval) == 1)
		return TIHTM_SUCCESS;
	return TIHTM_WRONG_TYPE;
}

void TiHtmAttribute::setIntValue(int _value)
{
	char buf[64] = {0};
	
	snprintf(buf, sizeof(buf), "%d", _value);
	setValue(buf);
}

void TiHtmAttribute::setDoubleValue(double _value)
{
	char buf[256] = {0};
	
	snprintf(buf, sizeof(buf), "%g", _value);
	setValue(buf);
}

const TiHtmAttribute* TiHtmAttribute::getNext() const
{
	// The sentinel have a value or name.
	if (!next || (next->value.empty() && next->name.empty()))
		return NULL;
	return next;
}

const TiHtmAttribute* TiHtmAttribute::getPrevious() const
{
	// The sentinel have a value or name.
	if (!prev || (prev->value.empty() && prev->name.empty()))
		return NULL;
	return prev;
}
void TiHtmAttribute::print(FILE* cfile, int /*depth*/, std::string* str) const
{
	std::string n, v;
	
	encodeString(name, &n);
	encodeString(value, &v);
	
	if (value.find('\"') == std::string::npos)
	{
		if (cfile)
		{
			fprintf(cfile, "%s=\"%s\"", n.c_str(), v.c_str());
		}
		if (str)
		{
			(*str) += n;
			(*str) += "=\"";
			(*str) += v;
			(*str) += "\"";
		}
	}
	else
	{
		if (cfile)
		{
			fprintf(cfile, "%s='%s'", n.c_str(), v.c_str());
		}
		if (str)
		{
			(*str) += n;
			(*str) += "='";
			(*str) += v;
			(*str) += "'";
		}
	}
}

void TiHtmAttribute::printValue(FILE *cfile, int depth) const
{
	//
}
// the scope of class TiHtmAttribute (end)

// the scope of class TiHtmAttributeSet (start)

TiHtmAttributeSet::TiHtmAttributeSet()
{
	sentinel.next = &sentinel;
	sentinel.prev = &sentinel;
}

TiHtmAttributeSet::~TiHtmAttributeSet()
{
	assert(sentinel.next = &sentinel);
	assert(sentinel.prev = &sentinel);
}

void TiHtmAttributeSet::add(TiHtmAttribute* addMe)
{
	assert(!find(addMe->getName()));
	
	addMe->next = &sentinel;
	addMe->prev = sentinel.prev;
	
	sentinel.prev->next = addMe;
	sentinel.prev = addMe;
}

void TiHtmAttributeSet::remove(TiHtmAttribute* removeMe)
{
	TiHtmAttribute* pnode;
	
	for (pnode = sentinel.next; pnode != &sentinel; pnode = pnode->next)
	{
		if (pnode == removeMe) // 这个地方并没有负责释放removeMe所占用内存，而是由调用函数负责释放的
		{
			pnode->prev->next = pnode->next;
			pnode->next->prev = pnode->prev;
			pnode->next = NULL;
			pnode->prev = NULL;
			return;
		}
	}
	assert(0);
}

TiHtmAttribute* TiHtmAttributeSet::find(const char* name) const
{
	for (TiHtmAttribute* pnode = sentinel.next; pnode != &sentinel; pnode = pnode->next)
	{
		if (pnode->name == name)
			return pnode;
	}
	return NULL;
}

TiHtmAttribute* TiHtmAttributeSet::findOrCreate(const char* _name)
{
	TiHtmAttribute* attr = find(_name);
	if (!attr)
	{
		attr = new TiHtmAttribute();
		add(attr);
		attr->setName(_name);
	}
	return attr;
}

TiHtmAttribute* TiHtmAttributeSet::find(const std::string& name) const
{
	for (TiHtmAttribute* pnode = sentinel.next; pnode != &sentinel; pnode = pnode->next)
	{
		if (pnode->name == name)
			return pnode;
	}
	return NULL;
}

TiHtmAttribute* TiHtmAttributeSet::findOrCreate(const std::string& _name)
{
	TiHtmAttribute* attr = find(_name);
	if (!attr)
	{
		attr = new TiHtmAttribute();
		add(attr);
		attr->setName(_name);
	}
	return attr;
}
// the scope of class TiHtmAttributeSet (end)

// the scope of class TiHtmElement

TiHtmElement::TiHtmElement(const char *_value) : TiHtmNode(TiHtmNode::TINYHTM_ELEMENT)
{
	firstChild = lastChild = NULL;
	value = _value;
}

TiHtmElement::TiHtmElement(const std::string &_value) : TiHtmNode(TiHtmNode::TINYHTM_ELEMENT)
{
	firstChild = lastChild = NULL;
	value = _value;
}

TiHtmElement::TiHtmElement(const TiHtmElement &copyElem) : TiHtmNode(TiHtmNode::TINYHTM_ELEMENT)
{
	firstChild = lastChild = NULL;
	copyElem.copyTo(this);
}

TiHtmElement::~TiHtmElement()
{
	clearThis();
}

const char* TiHtmElement::attribute(const char* name) const
{
	const TiHtmAttribute* pnode = attributeSet.find(name);
	if (pnode)
		return pnode->getValueStr();
	return NULL;
}
const char* TiHtmElement::attribute(const char* name, int* i) const
{
	const TiHtmAttribute* pnode = attributeSet.find(name);
	const char* result = NULL;
	
	if (pnode)
	{
		result = pnode->getValueStr();
		if (i)
		{
			pnode->queryIntValue(i);
		}
	}
	return result;
}

const char* TiHtmElement::attribute(const char* name, double* d) const
{
	const TiHtmAttribute* pnode = attributeSet.find(name);
	const char* result = NULL;
	
	if (pnode)
	{
		result = pnode->getValueStr();
		if (d)
		{
			pnode->queryDoubleValue(d);
		}
	}
	return result;
}

int TiHtmElement::QueryIntAttribute(const char* name, int* ival) const
{
	const TiHtmAttribute* pnode = attributeSet.find(name);
	if (!pnode)
		return TIHTM_NO_ATTRIBUTE;
	return pnode->queryIntValue(ival);
}

int TiHtmElement::queryUnsignedAttribute(const char* name, unsigned* _value) const
{
	const TiHtmAttribute* pnode = attributeSet.find(name);
	if (!pnode)
		return TIHTM_NO_ATTRIBUTE;
	
	int ival = 0;
	int result = pnode->queryIntValue(&ival);
	*_value = (unsigned)ival;
	return result;
}

int TiHtmElement::queryBoolAttribute(const char* name, bool* bval) const
{
	const TiHtmAttribute* pnode = attributeSet.find(name);
	if (!pnode)
		return TIHTM_NO_ATTRIBUTE;
	
	int result = TIHTM_WRONG_TYPE;
	if (stringEqual(pnode->getValue().c_str(), "true", true)
		|| stringEqual(pnode->getValue().c_str(), "yes", true)
		|| stringEqual(pnode->getValue().c_str(), "1", true))
	{
		*bval = true;
		result = TIHTM_SUCCESS;
	}
	else if (stringEqual(pnode->getValue().c_str(), "false", true)
		|| stringEqual(pnode->getValue().c_str(), "no", true)
		|| stringEqual(pnode->getValue().c_str(), "0", true))
	{
		*bval = false;
		result = TIHTM_SUCCESS;
	}
	
	return result;
}

int TiHtmElement::queryDoubleAttribute(const char* name, double* dval) const
{
	const TiHtmAttribute* pnode = attributeSet.find(name);
	if (!pnode)
		return TIHTM_NO_ATTRIBUTE;
	return pnode->queryDoubleValue(dval);
}

const std::string* TiHtmElement::attribute(const std::string& name) const
{
	const TiHtmAttribute* pnode = attributeSet.find(name);
	if (pnode)
		return &pnode->getValue();
	return NULL;
}

const std::string* TiHtmElement::attribute(const std::string& name, int* i) const
{
	const TiHtmAttribute* pnode = attributeSet.find(name);
	const std::string* result = NULL;
	
	if (pnode)
	{
		result = &pnode->getValue();
		if (i)
		{
			pnode->queryIntValue(i);
		}
	}
	return result;
}

const std::string* TiHtmElement::attribute(const std::string& name, double* d) const
{
	const TiHtmAttribute* pnode = attributeSet.find(name);
	const std::string* result = NULL;
	
	if (pnode)
	{
		result = &pnode->getValue();
		if (d)
		{
			pnode->queryDoubleValue(d);
		}
	}
	return result;
}

void TiHtmElement::setAttribute(const char* name, const char* value)
{
	TiHtmAttribute* pnode = attributeSet.findOrCreate(name);
	if (pnode)
	{
		pnode->setValue(value);
	}
}

void TiHtmElement::setAttribute(const char* name, int value)
{
	TiHtmAttribute* pnode = attributeSet.findOrCreate(name);
	if (pnode)
	{
		pnode->setIntValue(value);
	}
}

void TiHtmElement::setDoubleAttribute(const char* name, double value)
{
	TiHtmAttribute* pnode = attributeSet.findOrCreate(name);
	if (pnode)
	{
		pnode->setDoubleValue(value);
	}
}

void TiHtmElement::removeAttribute(const char *name)
{
	TiHtmAttribute* pnode = attributeSet.find(name);
	
	if (pnode)
	{
		attributeSet.remove(pnode);
		delete pnode;
	}
}

/// 如果element的第一个child不是text，则返回null，否则返回text的字符串

const char *TiHtmElement::getText() const
{
	const TiHtmNode *child = this->getFirstChild();
	if (child)
	{
		const TiHtmText *childText = child->toText();
		if (childText)
		{
			return childText->getValueStr();
		}
	}
	return NULL;
}


TiHtmNode *TiHtmElement::clone() const
{
	TiHtmElement *clone = new TiHtmElement(getValue());
	if (!clone)
		return NULL;
	
	copyTo(clone);
	return clone;
}

/// Print函数会递归调用，当element中包含element时，就会发生对自身的调用
void TiHtmElement::print(FILE *cfile, int depth) const
{
	assert(cfile);
	
	for (int i = 0; i < depth; i++)
	{
		fprintf(cfile, "    ");
	}
	
	fprintf(cfile, "<%s", value.c_str());
	
	const TiHtmAttribute* pattr;
	for (pattr = attributeSet.first(); pattr; pattr = pattr->getNext())
	{
		fprintf(cfile, " ");
		pattr->print(cfile, depth);
	}
	
	// There are 3 different formatting approaches:
	// 1) An element without children is printed as a <foo /> node
	// 2) An element with only a text child is printed as <foo> text </foo>
	// 3) An element with children is printed on multiple lines.
	TiHtmNode *pnode = NULL;
	if (!firstChild)
	{
		fprintf(cfile, " />");
	}
	else if (firstChild == lastChild && firstChild->toText())
	{
		fprintf(cfile, ">");
		firstChild->print(cfile, depth + 1);
		fprintf(cfile, "</%s>", value.c_str());
	}
	else
	{
		fprintf(cfile, ">");
		
		for (pnode = firstChild; pnode; pnode = pnode->nextSibling())
		{
			if (!pnode->toText())
			{
				fprintf(cfile, "\n");
			}
			pnode->print(cfile, depth + 1);
		}
		fprintf(cfile, "\n");
		
		for (int i = 0; i < depth; i++)
		{
			fprintf(cfile, "    ");
		}
		fprintf(cfile, "</%s>", value.c_str());
	}
}

void TiHtmElement::printValue(FILE *cfile, int depth) const
{
	assert(cfile);
	
	for (int i = 0; i < depth; i++)
	{
		fprintf(cfile, "    ");
	}
	
	//fprintf(cfile, "<%s", value.c_str());
	
	// There are 3 different formatting approaches:
	// 1) An element without children is printed as a <foo /> node
	// 2) An element with only a text child is printed as <foo> text </foo>
	// 3) An element with children is printed on multiple lines.
	TiHtmNode *pnode = NULL;
	if (!firstChild)
	{
		//fprintf(cfile, " />");
	}
	else if (firstChild == lastChild && firstChild->toText())
	{
		//fprintf(cfile, ">");
		firstChild->printValue(cfile, depth + 1);
		//fprintf(cfile, "</%s>", value.c_str());
	}
	else
	{
		//fprintf(cfile, ">");
		
		for (pnode = firstChild; pnode; pnode = pnode->nextSibling())
		{
			if (!pnode->toText())
			{
				fprintf(cfile, "\n");
			}
			pnode->printValue(cfile, depth + 1);
		}
		fprintf(cfile, "\n");
		
		for (int i = 0; i < depth; i++)
		{
			fprintf(cfile, "    ");
		}
		//fprintf(cfile, "</%s>", value.c_str());
	}
}

void TiHtmElement::copyTo(TiHtmElement *target) const
{
	TiHtmNode::copyTo(target);
}

void TiHtmElement::clearThis()
{
	clear();
	while (attributeSet.first())
	{
		TiHtmAttribute* pnode = attributeSet.first();
		attributeSet.remove(pnode);
		delete pnode;
	}
}

// the scope of class TiHtmComment (start)

TiHtmComment::TiHtmComment(const TiHtmComment& copy) : TiHtmNode(TiHtmNode::TINYHTM_COMMENT)
{
	copy.copyTo(this);
}

TiHtmComment& TiHtmComment::operator=(const TiHtmComment& base)
{
	clear();
	base.copyTo(this);
	return *this;
}

TiHtmNode* TiHtmComment::clone() const
{
	TiHtmComment* clone = new TiHtmComment();
	
	if (!clone)
		return NULL;
	
	copyTo(clone);
	return clone;
}

void TiHtmComment::print(FILE *cfile, int depth) const
{
	assert(cfile);
	for (int i = 0; i < depth; i++)
	{
		fprintf(cfile, "    ");
	}
	fprintf(cfile, "<!--%s-->", value.c_str());
}
void TiHtmComment::printValue(FILE *cfile, int depth) const
{
	// Empty
}

void TiHtmComment::copyTo(TiHtmComment* target) const
{
	TiHtmNode::copyTo(target);
}
// the scope of class TiHtmComment (end)

// the scope of class TiHtmText (start)

void TiHtmText::print(FILE *cfile, int depth) const
{
	assert(cfile);
	fprintf(cfile, "%s", value.c_str());
}

void TiHtmText::printValue(FILE *cfile, int depth) const
{
	fprintf(cfile, "%s", value.c_str());
}

TiHtmText *TiHtmText::clone() const
{
	TiHtmText *clone = NULL;
	clone = new TiHtmText("");
	
	if (!clone)
		return NULL;
	
	copyTo(clone);
	return clone;
}
// the scope of class TiHtmText (end)

// the scope of class TiHtmUnknown (start)

TiHtmUnknown* TiHtmUnknown::clone() const
{
	TiHtmUnknown* clone = new TiHtmUnknown();
	
	if (!clone)
		return NULL;
	copyTo(clone);
	return clone;
}

void TiHtmUnknown::printValue(FILE* cfile, int depth) const
{
	if (valueIsVisible)
		fprintf(cfile, "%s", value.c_str());
}

void TiHtmUnknown::print(FILE* cfile, int depth) const
{
	assert(cfile);
	for (int i = 0; i < depth; i++)
		fprintf(cfile, "    ");
	fprintf(cfile, "<%s>", value.c_str());
}

void TiHtmUnknown::copyTo(TiHtmUnknown* target) const
{
	TiHtmNode::copyTo(target);
}
// the scope of class TiHtmUnknown (start)

// the scope of class TiHtmDocument

TiHtmDocument::TiHtmDocument() : TiHtmNode(TiHtmNode::TINYHTM_DOCUMENT)
{
	tabsize = 4;
}

TiHtmDocument::TiHtmDocument(const char *filename) : TiHtmNode(TiHtmNode::TINYHTM_DOCUMENT)
{
	tabsize = 4;
	setValue(filename);
}

TiHtmDocument::TiHtmDocument(std::string filename) : TiHtmNode(TiHtmNode::TINYHTM_DOCUMENT)
{
	tabsize = 4;
	setValue(filename);
}

TiHtmDocument::TiHtmDocument(const TiHtmDocument &copy) : TiHtmNode(TiHtmNode::TINYHTM_DOCUMENT)
{
	copy.copyTo(this);
}

TiHtmDocument &TiHtmDocument::operator=(const TiHtmDocument &copy)
{
	clear();
	copy.copyTo(this);
	return *this;
}


bool TiHtmDocument::loadFile()
{
	return loadFile(value.c_str());
}

bool TiHtmDocument::loadFile(const std::string &filename)
{
	return loadFile(filename.c_str());
}

bool TiHtmDocument::loadFile(const char *filename)
{
	if (!filename && value.empty())
	{
		std::cout << "TiHtmDocument::loadFile(const char *filename) error" << std::endl;
		return false;
	}

	setValue(filename);
	FILE *file = TiHtmOpen(filename, "rb");

	if (file)
	{
		bool result = loadFile(file);
		fclose(file);
		return result;
	}
	else
	{
		return false;
	}
}

bool TiHtmDocument::loadFile(FILE *file)
{
	if (!file)
		exit(-1);
	
	long length = 0;
	fseek(file, 0, SEEK_END); // seek end of file
	length = ftell(file);
	fseek(file, 0, SEEK_SET); // sekk start of file

	char *buf = new char[length + 1];

	if (fread(buf, length, 1, file) != 1)
	{
		delete []buf;
		std::cout << "TiHtmDocument::loadFile(FILE *file) error" << std::endl;
		return false;
	}
	buf[length] = '\0';

	const char *p = buf;
	char *q = buf;
	const char CR = 0x0d; // '\r'
	const char LF = 0x0a; // '\n'

	while (*p)
	{
		if (*p == CR)
		{
			*q++ = LF;
			if (*p == LF)
				p++;
		}
		else
		{
			*q++ = *p++;
		}
	}
	assert(q <= (buf + length));
	*q = '\0';
	
	// Now start parse the xml file
	parse(buf, 0);

	delete []buf;
	return true;
}

bool TiHtmDocument::saveFile() const
{
	return saveFile(getValue());
}

bool TiHtmDocument::saveFile(const char *filename) const
{
	FILE *file = TiHtmOpen(filename, "w");
	if (file)
	{
		bool result = saveFile(file);
		fclose(file);
		return result;
	}
	
	return false;
}

bool TiHtmDocument::saveFile(const std::string &filename) const
{
	return saveFile(filename.c_str());
}

bool TiHtmDocument::saveFile(FILE *file) const
{
	print(file, 0);
	return true;
}

void TiHtmDocument::print(FILE *cfile, int depth) const
{
	assert(cfile);
	for (const TiHtmNode *pnode = getFirstChild(); pnode; pnode = pnode->nextSibling())
	{
		pnode->print(cfile, depth);
		fprintf(cfile, "\n");
	}
}

void TiHtmDocument::printValue(FILE *cfile, int depth) const
{
	assert(cfile);

	for (const TiHtmNode *pnode = getFirstChild(); pnode; pnode = pnode->nextSibling())
	{
		pnode->printValue(cfile, depth);
		fprintf(cfile, "\n");
	}
}

TiHtmNode *TiHtmDocument::clone() const
{
	TiHtmDocument *clone = new TiHtmDocument();
	if (!clone)
	{
		return NULL;
	}
	
	copyTo(clone);
	return clone;
}

void TiHtmDocument::copyTo(TiHtmDocument *target) const
{
	TiHtmNode::copyTo(target);
	
	target->tabsize = tabsize;
	
	TiHtmNode *pnode = NULL;
	for (pnode = firstChild; pnode; pnode = pnode->nextSibling())
	{
		target->linkEndChild(pnode->clone());
	}
}

