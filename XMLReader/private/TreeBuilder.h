#pragma once

#include "symbol.h"
#include "XMLReader.h"

#include <map>
#include <list>
#include <string>
#include <stack>

namespace xml_reader
{
	struct IBuilder;
	struct TreeBuilder;

	struct Builder
	{
		IBuilder* m_builder = nullptr;
		void DoBuildStep(TreeBuilder& builder);
	};

	struct TreeBuilder
	{
		std::list<Node*>& m_generatedNodes;
		std::list<Node*> m_rootNodes;

		std::stack<Builder> m_builders;

		bool BuildTree(scripting::ISymbol* rootSymbol);
		Node* GetNewNode();

		TreeBuilder(std::list<Node*>& nodesContainer);
	};

	struct IBuilder
	{
		enum BuilderState
		{
			Pending,
			Failed,
			Done,
		};

		scripting::ISymbol* m_rootSymbol = nullptr;
		BuilderState m_state = BuilderState::Pending;
		std::list<Node*> m_generatedNodes;

		virtual void DoBuildStep(TreeBuilder& builder) = 0;
		virtual void Dispose() = 0;

	protected:
		IBuilder(Builder& colladaBuilder);
	};
}