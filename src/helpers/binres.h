#pragma  once

namespace Flug {
	class BinRes {
	public:
		BinRes ();
		virtual ~BinRes();

		void load (const std::string & path);

		const uint8_t * data () const;
		size_t size () const;
	protected:
		std::vector<uint8_t> m_data;
	};
}