#include "XMLReader.h"

#include "XMLReader.h"
#include "dataLib.h"
#include "parserTable.h"
#include "parser.h"
#include "grammar.h"
#include "codeSource.h"
#include "TreeBuilder.h"

#include "XMLReaderMeta.h"

#include "BaseObject.h"

#include <map>
#include <queue>

namespace
{
	xml_reader::IXMLReader* m_reader = nullptr;

	std::string ReadGrammar()
	{
		data::DataLib& dataLib =  data::GetLibrary();

		std::string grammar = dataLib.ReadFileById("collada_grammar");

		return grammar;
	}

	class XMLReader : public BaseObject, public xml_reader::IXMLReader
	{
	public:

		struct ColladaFile
		{
			scripting::CodeSource m_codeSource;
			scripting::ISymbol* m_parsed = nullptr;
		};

		std::map<std::string, ColladaFile> m_colladaFiles;

		scripting::Grammar m_grammar;
		scripting::ParserTable m_parserTable;
		scripting::Parser* m_parser = nullptr;

		XMLReader(const std::string& grammar) :
			m_grammar(grammar),
			BaseObject(xml_reader::XMLReaderMeta::GetInstance())
		{
			m_grammar.GenerateParserStates();
			m_grammar.GenerateParserTable(m_parserTable);

			bool valid = m_parserTable.Validate();

			m_parser = new scripting::Parser(m_grammar, m_parserTable);
		}
		~XMLReader()
		{
			delete m_parser;
			m_parser = nullptr;
		}

		scripting::ISymbol* ReadColladaFile(const std::string& file) override
		{
			std::map<std::string, ColladaFile>::iterator it = m_colladaFiles.find(file);

			if (it != m_colladaFiles.end()) {
				return it->second.m_parsed;
			}

			data::DataLib& lib = data::GetLibrary();
			std::string fileData = lib.ReadFileByPath(file);

			m_colladaFiles.insert(std::pair<std::string, ColladaFile>(file, ColladaFile()));
			ColladaFile& colladaFile = m_colladaFiles[file];

			colladaFile.m_codeSource.m_code = fileData;
			colladaFile.m_codeSource.Tokenize();
			colladaFile.m_codeSource.TokenizeForColladaReader();

			colladaFile.m_parsed = m_parser->Parse(colladaFile.m_codeSource);

			return colladaFile.m_parsed;
		}

		bool ConstructColladaTree(scripting::ISymbol* symbol, std::list<xml_reader::Node*>& nodes, std::list<xml_reader::Node*>& allNodes) override;
	};
	


	bool XMLReader::ConstructColladaTree(scripting::ISymbol* symbol, std::list<xml_reader::Node*>& nodes, std::list<xml_reader::Node*>& allNodes)
	{
		xml_reader::TreeBuilder builder(allNodes);
		bool res = builder.BuildTree(symbol);
		nodes = builder.m_rootNodes;
		return res;
	}

}

xml_reader::IXMLReader* xml_reader::GetReader()
{
	return m_reader;
}

void xml_reader::Boot()
{
	if (m_reader)
	{
		return;
	}
	
	std::string grammar = ReadGrammar();
	m_reader = new XMLReader(grammar);
}

void xml_reader::FindChildNodes(const Node* rootNode, std::function<bool(const Node*)> predicate, std::list<const Node*>& nodesFound)
{
	std::queue<const Node*> nodesToCheck;
	nodesToCheck.push(rootNode);

	while (!nodesToCheck.empty())
	{
		const Node* cur = nodesToCheck.front();
		nodesToCheck.pop();

		if (predicate(cur))
		{
			nodesFound.push_back(cur);
		}

		for (std::list<Node*>::const_iterator it = cur->m_children.begin();
			it != cur->m_children.end(); ++it)
		{
			nodesToCheck.push(*it);
		}
	}
}

const xml_reader::Node* xml_reader::FindChildNode(const Node* rootNode, std::function<bool(const Node*)> predicate)
{
	std::queue<const Node*> nodesToCheck;
	nodesToCheck.push(rootNode);

	while (!nodesToCheck.empty())
	{
		const Node* cur = nodesToCheck.front();
		nodesToCheck.pop();

		if (predicate(cur))
		{
			return cur;
		}

		for (std::list<Node*>::const_iterator it = cur->m_children.begin();
			it != cur->m_children.end(); ++it)
		{
			nodesToCheck.push(*it);
		}
	}
	return nullptr;
}

void xml_reader::AssignParents(const std::list<xml_reader::Node*>& nodes)
{
	using namespace xml_reader;

	std::set<Node*> processed;

	std::queue<Node*> toProcess;

	for (auto it = nodes.begin(); it != nodes.end(); ++it)
	{
		toProcess.push(*it);
	}

	while (!toProcess.empty())
	{
		Node* cur = toProcess.front();
		toProcess.pop();

		if (processed.contains(cur))
		{
			continue;
		}

		for (auto it = cur->m_children.begin(); it != cur->m_children.end(); ++it)
		{
			Node* curChild = *it;
			curChild->m_parent = cur;
			toProcess.push(curChild);
		}
		processed.insert(cur);
	}
}