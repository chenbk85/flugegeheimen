#pragma once

namespace Flug {
	class Csv {
	public:
		Csv ();
		Csv (const std::string & path);
		virtual ~Csv ();

		void load (const std::string & path);
		void save (const std::string & path);



	protected:
	private:
		DataTable m_dataTable;
	};
}
