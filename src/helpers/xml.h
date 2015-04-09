#pragma once

namespace Flug {

	class Xml {
	public:
		Xml();

		Xml(const std::string &path);

		virtual ~Xml();

		void loadData(const std::string &path);

		rapidxml::xml_node<> *getNode(const std::list<std::string> &path);

		rapidxml::xml_attribute<> *getAttribute(rapidxml::xml_node<> *node, const std::string &name);

		template<typename T>
		void get(const std::string &xpath, T &val) {

			size_t posPtr, nodeStart;
			std::list<std::string> parsedPath;
			std::string attrName = "";
			char prevSeparator = '/';
			char currentSeparator;
			bool hasAttributeMark = false;


			for (posPtr = 0, nodeStart = 0; posPtr != std::string::npos;
				 posPtr = xpath.find_first_of("/@", nodeStart)) {
				currentSeparator = xpath[posPtr];
				if (posPtr == 0) {
					nodeStart = 1;
					continue;
				}

				if (prevSeparator == '/') {
					std::string snode(xpath.c_str() + nodeStart, xpath.c_str() + posPtr);
					parsedPath.push_back(snode);
				} else {
					throw std::runtime_error(("Wrong separators sequence in '" + xpath + "'").c_str());
				}

				if (currentSeparator == '@') {
					hasAttributeMark = true;
				}

				nodeStart = posPtr + 1;
				prevSeparator = currentSeparator;

			}

			if (nodeStart < xpath.size()) {
				std::string snode(xpath.c_str() + nodeStart, xpath.c_str() + xpath.size());
				if (hasAttributeMark) {
					attrName = snode;
				} else {
					parsedPath.push_back(snode);
				}
			}

			std::stringstream ss;
			rapidxml::xml_node<> *currentNode = getNode(parsedPath);
			if (hasAttributeMark) {
				rapidxml::xml_attribute<> *currentAttr = getAttribute(currentNode, attrName);
				ss << currentAttr->value();
				ss >> val;
			} else {
				ss << currentNode->value();
				ss >> val;
			}
		};

	protected:
		std::vector<char> m_data;
		rapidxml::xml_document<> m_doc;
	};

}
