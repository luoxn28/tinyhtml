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
	if (pnode->getType() == TiHtmNode::TINYHTM_DOCUMENT)
	{
		delete pnode;
		std::cout << "TiHtmNode::linkEndChild() error" << std::endl;
		return NULL;
	}
	
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
	
	const char *htmlHeader = "<html";
	const char *commentHeader = "<!--";
	const char *dtdHeader = "<!";
	
	if (stringEqual(p, htmlHeader, true))
	{
		//returnNode = new TiXmlDeclaration();
	}
	else if (stringEqual(p, commentHeader, true))
	{
		//returnNode = new TiXmlComment();
	}
	else if (stringEqual(p, dtdHeader, true))
	{
		//returnNode = new TiXmlUnknown();
	}
	else if (isAlpha(*(p+1)) || *(p+1) == '_')
	{
		//returnNode = new TiXmlElement();
	}
	else
	{
		//returnNode = new TiXmlElement();
	}
	
	if (returnNode)
	{
		returnNode->parent = this;
	}
	
	return returnNode;
}

// the scope of class TiHtmDocument

bool TiHtmDocument::loadFile()
{
	return loadFile(value.c_str());
}

bool TiHtmDocument::loadFile(std::string filename)
{
	return loadFile(filename.c_str());
}

bool TiHtmDocument::loadFile(const char *filename)
{
	if (!filename && value.empty())
	{
		TiHtmCout << "filename is null && value is empty" << TiHtmEndl;
		exit(-1);
	}

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
	fseek(file, 0, SEEK_END);
	length = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *buf = new char[length + 1];

	if (fread(buf, length, 1, file) != 1)
	{
		exit (-1);
	}
	buf[length] = '\0';

	TiHtmCout << buf << TiHtmEndl;

	delete []buf;
}

