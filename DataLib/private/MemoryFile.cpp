#include "MemoryFile.h"

#include <stdio.h>

void data::MemoryFile::Write(size_t position, size_t size, void* data)
{
	{
		size_t chunksNeeded = (position + size) / m_chunkSize;
		size_t offset = (position + size) - chunksNeeded * m_chunkSize;
		if (offset > 0)
		{
			++chunksNeeded;
		}

		while (m_contents.size() < chunksNeeded)
		{
			char* newChunk = new char[m_chunkSize];
			m_contents.push_back(newChunk);
		}
	}

	std::list<void*>::iterator curChunkIt = m_contents.begin();
	size_t curChunkOffset = 0;
	
	{
		size_t startChunkIndex = position / m_chunkSize;
		size_t startChunkOffset = position - startChunkIndex * m_chunkSize;
		curChunkOffset = startChunkOffset;

		for (size_t i = 0; i < startChunkIndex; ++i)
		{
			++curChunkIt;
		}
	}

	size_t written = 0;
	while (written < size)
	{
		char* charDataDst = static_cast<char*>(*curChunkIt) + curChunkOffset;
		char* charDataSrc = static_cast<char*>(data) + written;

		size_t toWrite = std::min(size - written, m_chunkSize - curChunkOffset);

		memcpy(charDataDst, charDataSrc, toWrite);

		curChunkOffset += toWrite;

		{
			std::list<void*>::iterator nextIt = curChunkIt;
			++nextIt;
			if (nextIt == m_contents.end())
			{
				m_lastChunkSize = std::max(m_lastChunkSize, curChunkOffset);
			}
		}

		while (curChunkOffset >= m_chunkSize)
		{
			++curChunkIt;
			curChunkOffset -= m_chunkSize;
		}

		written += toWrite;
	}
}

size_t data::MemoryFile::Read(size_t position, size_t size, void* data)
{
	if (position >= GetFileSize())
	{
		return 0;
	}

	size_t sizeToRead = std::min(size, GetFileSize() - position);

	size_t startChunk = position / m_chunkSize;
	size_t startChunkOffset = position - startChunk * m_chunkSize;

	std::list<void*>::iterator curChunkIt = m_contents.begin();
	for (int i = 0; i < startChunk; ++i)
	{
		++curChunkIt;
	}
	size_t curChunkOffset = startChunkOffset;

	size_t read = 0;
	while (read < sizeToRead)
	{
		char* charDataDst = static_cast<char*>(data) + read;
		char* charDataSrc = static_cast<char*>(*curChunkIt) + curChunkOffset;
		size_t toRead = std::min(sizeToRead - read, m_chunkSize - curChunkOffset);

		memcpy(charDataDst, charDataSrc, toRead);

		read += toRead;
		curChunkOffset += toRead;

		while (curChunkOffset >= m_chunkSize)
		{
			++curChunkIt;
			curChunkOffset -= m_chunkSize;
		}
	}

	return read;
}

size_t data::MemoryFile::GetFileSize()
{
	if (m_contents.empty())
	{
		return 0;
	}

	return (m_contents.size() - 1) * m_chunkSize + m_lastChunkSize;
}

data::MemoryFile::MemoryFile()
{
}

data::MemoryFile::~MemoryFile()
{
	for (auto it = m_contents.begin(); it != m_contents.end(); ++it)
	{
		delete[] (*it);
	}
}

data::MemoryFileWriter::MemoryFileWriter(MemoryFile& file) :
	m_file(file)
{
}

void data::MemoryFileWriter::Write(void* data, size_t size)
{
	m_file.Write(m_position, size, data);
	m_position += size;
}

data::MemoryFileReader::MemoryFileReader(MemoryFile& file) :
	m_file(file)
{
}

size_t data::MemoryFileReader::Read(void* data, size_t size)
{
	size_t read = m_file.Read(m_position, size, data);
	m_position += read;

	return read;
}


void data::MemoryFile::SaveToFile(const std::string& file)
{
	FILE* f = nullptr;
	fopen_s(&f, file.c_str(), "wb");

	if (!f)
	{
		return;
	}

	int index = 0;
	for (auto it = m_contents.begin(); it != m_contents.end(); ++it)
	{
		size_t size = m_chunkSize;

		if (index + 1 == m_contents.size())
		{
			size = std::min(size, m_lastChunkSize);
		}

		void* chunk = *it;
		fwrite(chunk, sizeof(char), size, f);
	}

	fclose(f);
}

void data::MemoryFile::RestoreFromFile(const std::string& file)
{
	FILE* f = nullptr;
	fopen_s(&f, file.c_str(), "rb");

	if (!f)
	{
		return;
	}

	size_t read = m_chunkSize;

	char* buf = new char[m_chunkSize];

	while (read > 0)
	{
		read = fread_s(buf, m_chunkSize, sizeof(char), m_chunkSize, f);

		if (read > 0)
		{
			m_contents.push_back(buf);
			m_lastChunkSize = read;
			buf = new char[m_chunkSize];
		}
	}

	delete[] buf;

	fclose(f);
}