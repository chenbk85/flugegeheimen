#include "../stdafx.h"
#include "xml.h"

namespace Flug {


Xml::Xml () {
}

Xml::Xml(const std::string & path) {
}

Xml::~Xml () {

}

void Xml::loadData(const std::string & path) {
	std::ifstream testFile(path.c_str(), std::ios::binary);
	m_data.assign((std::istreambuf_iterator<char>(testFile)),
									std::istreambuf_iterator<char>());
	m_doc.parse<0> (m_data.data());
}

rapidxml::xml_node<> * Xml::getNode (const std::list<std::string> & path) {
	rapidxml::xml_node<> * currentNode;
	currentNode = m_doc.first_node(path.front().c_str());
	if (!currentNode) {
		throw std::runtime_error(("Wrong xml path: there is no " + path.front() + " node").c_str());
	}
	for (std::list<std::string>::const_iterator iter = ++(path.begin());
			iter != path.end(); iter++) {
		currentNode = currentNode->first_node(iter->c_str());
		if (!currentNode) {
			throw std::runtime_error(("Wrong xml path: there is no " + *iter + " node").c_str());
		}
	}
	return currentNode;
}

rapidxml::xml_attribute<> * Xml::getAttribute (rapidxml::xml_node<> * node, const std::string & name) {
	if (!node) {
		throw std::runtime_error("Xml::getAttribute: node is NULL");
	}
	rapidxml::xml_attribute<> * attr = node->first_attribute(name.c_str());
	if (!attr) {
		throw std::runtime_error("Xml::getAttribute: attr is NULL");
	}
	return attr;
}



}
