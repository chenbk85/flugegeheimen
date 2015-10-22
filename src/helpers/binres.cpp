#include "../stdafx.h"
#include "binres.h"

namespace Flug {
	BinRes::BinRes() {

	}

	BinRes::~BinRes() {
	}

	void BinRes::load (std::string const & path) {
		std::ifstream testFile(path.c_str(), std::ios::binary);
		if (!testFile.is_open()) {
			throw std::runtime_error(("There is no such resource: " + path).c_str());
		}
		m_data.assign((std::istreambuf_iterator<char>(testFile)), std::istreambuf_iterator<char>());
	}

	size_t BinRes::size () const {
		return m_data.size();
	}

	const uint8_t* BinRes::data () const {
		return m_data.data();
	}
	
}