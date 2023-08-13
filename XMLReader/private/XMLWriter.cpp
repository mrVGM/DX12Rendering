#include "XMLWriter.h"

#include <sstream>
#include <stack>

namespace
{
	struct Builder;
	struct BuilderStack
	{
		std::stack<Builder*> m_stack;

		void BuildStep();
	};
	struct Builder
	{
		enum State
		{
			FirstCall,
			ChildrenProcessing,
			Finished
		};

		BuilderStack& m_builderStack;
		std::string m_prefix;
		xml_writer::Node& m_node;

		std::list<Builder*> m_childBuilders;

		State m_state = FirstCall;

		std::stringstream m_ss;

		Builder(BuilderStack& builderStack, xml_writer::Node& node, const std::string& prefix) :
			m_builderStack(builderStack),
			m_node(node),
			m_prefix(prefix)
		{
		}

		void BuildStep()
		{
			switch (m_state)
			{
			case Builder::State::FirstCall:
				m_ss << m_prefix << "<" << m_node.m_tagName;

				for (auto it = m_node.m_tagProps.begin(); it != m_node.m_tagProps.end(); ++it)
				{
					m_ss << " " << it->first << "=" << "\"" << it->second << "\"";
				}

				m_ss << ">";

				if (!m_node.m_content.empty())
				{
					m_ss << m_node.m_content
						<< "</" << m_node.m_tagName << ">" << std::endl;

					m_state = Finished;
					break;
				}

				m_ss << std::endl;

				m_state = ChildrenProcessing;

				for (auto it = m_node.m_children.begin(); it != m_node.m_children.end(); ++it)
				{
					Builder* b = new Builder(m_builderStack, *it, m_prefix + "    ");
					m_childBuilders.push_back(b);
				}

				for (auto it = m_childBuilders.rbegin(); it != m_childBuilders.rend(); ++it)
				{
					m_builderStack.m_stack.push(*it);
				}
				break;
			case Builder::State::ChildrenProcessing:
				for (auto it = m_childBuilders.begin(); it != m_childBuilders.end(); ++it)
				{
					m_ss << (*it)->m_ss.str();
					delete (*it);
				}

				m_childBuilders.clear();

				m_ss << m_prefix << "</" << m_node.m_tagName << ">" << std::endl;

				m_state = State::Finished;

				break;
			}
		}
	};

	void BuilderStack::BuildStep()
	{
		Builder* b = m_stack.top();
		if (b->m_state == Builder::State::Finished)
		{
			m_stack.pop();
			return;
		}

		b->BuildStep();
	}

	std::string NodeToString(const std::string& prefix, xml_writer::Node& node)
	{
		BuilderStack builderStack;
		
		Builder rootBuilder(builderStack, node, prefix);

		builderStack.m_stack.push(&rootBuilder);

		while (!builderStack.m_stack.empty())
		{
			builderStack.BuildStep();
		}

		return rootBuilder.m_ss.str();
	}
}

std::string xml_writer::Node::ToString()
{
	std::stringstream ss;
	ss << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
	std::string res = NodeToString("", *this);
	ss << res;
	return ss.str();
}