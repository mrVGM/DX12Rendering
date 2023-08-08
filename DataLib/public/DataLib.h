#pragma once

#include "json.hpp"

#include <stdio.h>
#include <string>
#include <vector>

namespace data
{
	struct BinReader
	{
		FILE* m_file = nullptr;
		unsigned int m_filePos = 0;

		BinReader(const std::string& fileName);
		~BinReader();

		unsigned int Read(char* buf, int bufSize);
	};

	struct BinWriter
	{
		FILE* m_file = nullptr;
		unsigned int m_filePos = 0;

		BinWriter(const std::string& fileName);
		~BinWriter();

		unsigned int Write(char* buf, int bufSize);
	};

	struct BinChunk
	{
		unsigned int m_size = 0;
		char* m_data = nullptr;

		void Read(BinReader& br);
		void Write(BinWriter& bw);

		~BinChunk();
	};

	class DataLib
	{
		std::string m_data;
		nlohmann::json m_json;
		std::vector<nlohmann::json> m_fileEntries;

	public:
		DataLib(const char* dir, const char* name = "lib.json");
		inline bool IsValid() const { return m_data.length() > 0; }
		nlohmann::json GetFileEntry(const char* id) const;
		const std::vector<nlohmann::json>& GetFileEntries () const;

		std::string ReadFileById(const char* id);
		std::string ReadFileByPath(std::string path);

		void WriteBinFile(const char* id, const void *data, size_t size);
		size_t ReadBinFile(const char* id, void *data, size_t size);

		const std::string& GetRootDir() const;
	};

	bool Init(const char* dir);
	void Deinit();

	DataLib& GetLibrary();
}