#pragma once

namespace Flug {


	class Kernel {
	public:
		Kernel();

		virtual ~Kernel();

		void main();

		void loadConfig(const std::string &confPath);

	protected:
	private:
	};


}
