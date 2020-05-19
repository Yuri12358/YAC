#ifndef YAC_YAC_HPP
#define YAC_YAC_HPP

#include<YAC/compressor.hpp>
#include<YAC/extractor.hpp>
#include<fstream>
#include<string>

namespace yac {
	class Yac {
		enum class Operation {
			None,
			Compress,
			Extract
		};

		yac::Compressor m_compressor;
		yac::Extractor m_extractor;
		Operation m_op = Operation::None;

		std::ifstream m_iFile;
		std::ofstream m_oFile;

		std::string m_processArgs(int argc, const char ** argv);
		void m_printUsage(const std::string & progName);
		void m_openFiles(const std::string & filename);

	public:
		Yac(int argc, const char ** argv);
	};
}

#endif // YAC_YAC_HPP
