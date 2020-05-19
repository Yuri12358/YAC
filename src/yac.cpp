#include<YAC/yac.hpp>
#include<YAC/istream_byte_source.hpp>
#include<YAC/ostream_byte_sink.hpp>
#include<fstream>
#include<stdexcept>
#include<cstdio>
#include<string_view>

using namespace std::literals;

yac::Yac::Yac(int argc, const char ** argv) {
	auto filename = m_processArgs(argc, argv);
	try {
		IStreamByteSource in(m_iFile);
		OStreamByteSink out(m_oFile);
		if (m_op == Operation::Compress) {
			m_openFiles(filename);
			m_compressor.compress(in, out);
		} else if (m_op == Operation::Extract) {
			m_openFiles(filename);
			m_extractor.extract(in, out);
		}
	} catch (const std::runtime_error & e) {
		printf("Fatal error: %s\n", e.what());
	}
}

std::string yac::Yac::m_processArgs(int argc, const char ** argv) {
	m_op = Operation::None;
	if (argc == 3) {
		const std::string_view operationFlag = argv[1];
		if (operationFlag == "-c") {
			m_op = Operation::Compress;
			return argv[2];
		} else if (operationFlag == "-x") {
			m_op = Operation::Extract;
			const std::string_view filename = argv[2];
			if (filename.size() > 4 and filename.substr(filename.size() - 4) == ".yac") {
				return std::string{filename.substr(0, filename.size() - 4)};
			}
		}
	}
	m_printUsage(argv[0]);
	return "";
}

void yac::Yac::m_printUsage(const std::string & progName) {
	printf(
		"%s - Yet Another Compressor.\n"
		"Options:\n"
		"\t-c <file>    \t- compress file\n"
		"\t-x <file>.yac\t- extract file\n",
		progName.c_str()
	);
}

void yac::Yac::m_openFiles(const std::string & filename) {
	std::string iFileName = filename;
	std::string oFileName = filename;
	if (m_op == Operation::Compress) {
		oFileName += ".yac";
	} else if (m_op == Operation::Extract) {
		iFileName += ".yac";
	}
	m_iFile.open(iFileName, std::ifstream::in | std::ifstream::binary);
	if (!m_iFile.is_open()) {
		throw std::runtime_error("Error while opening file");
	}
	m_oFile.open(oFileName, std::ofstream::out
		| std::ofstream::trunc | std::ofstream::binary);
}

