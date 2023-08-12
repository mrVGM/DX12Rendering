#pragma once

#include <list>
#include <string>

namespace data
{
	class MemoryFile;

	class MemoryFileWriter
	{
	private:
		MemoryFile& m_file;
		size_t m_position = 0;
	public:
		MemoryFileWriter(MemoryFile& file);
		MemoryFileWriter(const MemoryFileWriter& other) = delete;
		MemoryFileWriter& operator=(const MemoryFileWriter& other) = delete;

		void Write(void* data, size_t size);
	};

	class MemoryFileReader
	{
	private:
		MemoryFile& m_file;
		size_t m_position = 0;
	public:
		MemoryFileReader(MemoryFile& file);
		MemoryFileReader(const MemoryFileReader& other) = delete;
		MemoryFileReader& operator=(const MemoryFileReader& other) = delete;

		size_t Read(void* data, size_t size);
	};

	class MemoryFile
	{
		friend class MemoryFileWriter;
		friend class MemoryFileReader;

		static const size_t m_chunkSize = 1024;
		std::list<void*> m_contents;
		size_t m_lastChunkSize = 0;

		void Write(size_t position, size_t size, void* data);
		size_t Read(size_t position, size_t size, void* data);

		size_t GetFileSize();

	public:
		MemoryFile();
		MemoryFile(const MemoryFile& other) = delete;
		MemoryFile& operator=(const MemoryFile& other) = delete;
		~MemoryFile();

		void SaveToFile(const std::string& file);
		void RestoreFromFile(const std::string& file);
	};
}