#include "../stdafx.h"
#include "csv.h"

namespace Flug {
	Csv::Csv () {
	}

	Csv::~Csv() {
	}

	void Csv::load(const std::string & path) {
		m_data.clear();
		std::ifstream file;
		file.open(path);
		m_data.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

		file.close();
	}

	std::list <std::vector<std::string > >::iterator Csv::begin() {
		return m_data.begin();
	}

	std::list <std::vector<std::string > >::iterator Csv::end() {
		return m_data.end();
	}


}
