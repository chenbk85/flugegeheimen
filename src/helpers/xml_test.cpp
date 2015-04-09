#include "../stdafx.h"
#include "xml.h"
#include "gtest/gtest.h"

namespace Flug {

	TEST (Xml, LoadFromFile) {
		Xml xml;
		xml.loadData("res/test.xml");
	}

	TEST (Xml, GetStringByPath) {
		Xml xml;
		xml.loadData("res/test.xml");
		std::string str = "DEFAULT_VALUE";
		xml.get("/test/string", str);

		EXPECT_STREQ(str.c_str(), "some_string_out_there");
	}

	TEST (Xml, GetIntByPath) {
		Xml xml;
		xml.loadData("res/test.xml");
		int i = 2345;
		xml.get("/test/integer", i);

		EXPECT_EQ(i, 100);
	}

	TEST (Xml, GetIntByPathWithTrailingSeparator) {
		Xml xml;
		xml.loadData("res/test.xml");
		int i = 2345;
		xml.get("/test/integer/", i);

		EXPECT_EQ(i, 100);
	}


	TEST (Xml, GetIntAttributeByPath) {
		Xml xml;
		xml.loadData("res/test.xml");
		int i = 624;
		xml.get("/test/attr@int_attr", i);

		EXPECT_EQ(i, 1234);
	}

	TEST (Xml, GetNodeByListPath) {
		Xml xml;
		xml.loadData("res/test.xml");
		std::list<std::string> testPath = {"test", "integer"};
		rapidxml::xml_node<> *node = xml.getNode(testPath);
		EXPECT_NE(node, nullptr);
	}

}
