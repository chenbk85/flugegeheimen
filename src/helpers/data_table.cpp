#pragma once

namespace Flug {
	class DataTable {
	public:
		enum CellType {CT_INT, CT_STRING, CT_FLOAT, CT_UNKNOWN};
		DataTable ();
		virtual ~DataTable ();

		void setFormat (const std::list <CellType> & list);
		void loadData ();
		void saveData ();

		void addRow ();
	protected:
	private:
	};
}
