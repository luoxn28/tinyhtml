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

/// 如果element的第一个child不是text，则返回null，否则返回text的字符串
/*
const char *TiHtmElement::getText() const
{
	const TiHtmNode *child = this->getFirstChild();
	if (child)
	{
		const TiHtmText *childText = child->toText();
		if (childText)
		{
			//return childText->getValue();
		}
	}
	return NULL;
}
*/

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

void TiHtmElement::copyTo(TiHtmElement *target) const
{
	TiHtmNode::copyTo(target);
	
}

void TiHtmElement::clearThis()
{
	clear();
}

// the scope of class TiHTmText

void TiHtmText::print(FILE *cfile, int depth) const
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

