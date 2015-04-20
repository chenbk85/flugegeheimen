#pragma once

namespace Flug {
	class Csv {
	public:
		Csv ();
		Csv (const std::string & path);
		virtual ~Csv ();

		void load (const std::string & path);
		void save (const std::string & path);

		std::list<std::vector<std::string> >::iterator begin ();
		std::list<std::vector<std::string> >::iterator end ();

	protected:
		std::list < std::vector <std::string> > m_data;
	private:
	};
}