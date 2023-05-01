#include "ColladaReader.h"

#include "ColladaReaderMeta.h"
#include "Collada.h"
#include "ColladaEntities.h"

namespace
{
	collada::ColladaReader* m_instance = nullptr;
}

collada::ColladaReader::ColladaReader() :
	BaseObject(ColladaReaderMeta::GetInstance())
{
	collada::GetReader();
}

collada::ColladaReader::~ColladaReader()
{
	collada::ReleaseColladaReader();
	m_instance = nullptr;
}

collada::ColladaReader& collada::ColladaReader::GetInstance()
{
	if (!m_instance)
	{
		m_instance = new ColladaReader();
	}

	return *m_instance;
}


scripting::ISymbol* collada::ColladaReader::ReadColladaFile(const std::string& file)
{
	IColladaReader* realReader = collada::GetReader();
	return realReader->ReadColladaFile(file);
}

bool collada::ColladaReader::ConstructColladaTree(scripting::ISymbol* rootSymbol, std::list<collada::ColladaNode*>& nodes, std::list<collada::ColladaNode*>& allNodes)
{
	IColladaReader* realReader = collada::GetReader();
	return realReader->ConstructColladaTree(rootSymbol, nodes, allNodes);
}
