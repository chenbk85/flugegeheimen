#pragma  once

namespace Flug {
	class BinRes {
	public:
		BinRes ();
		virtual ~BinRes();

		void load (const std::string & path);

		const char * data () const;
		size_t size () const;
	protected:
		std::vector<char> m_data;
	};
}