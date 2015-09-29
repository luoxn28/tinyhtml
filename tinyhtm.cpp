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

