#include "stdafx.h"
#include "Brain.h"
#include "Manager/Action/CommandMgr.h"
#include "Actions/Command.h"
#include "Entity/Entity.h"
#include "Manager/Level/LevelMgr.h"
#include "Graph/GraphAlgorithms.h"
#include "Graph/AStarHeuristicPolicies.h"
#include "Manager/Physic/PhysicMgr.h"

std::vector<const char*> IntentoToCommandName =
{
	"CommandMoveUp",
	"CommandMoveRight",
	"CommandMoveDown",
	"CommandMoveLeft",
	"CommandClean",
	"CommandPickUp",
	"CommandExplore"
};

Brain::Brain(Entity* ent)
	:m_entity(ent),
	m_exploSparseGraph(false)
{
	m_energy = 1;
	m_exploreTime = 10.0f;
	m_timer = 0.0f;
	initGraph();
}

Brain::~Brain()
{
}

const bool Brain::executeAction()
{
	if (m_intentions.empty()) {
		return false;
	}
	else {
		int id;
		Command* cmd = CommandMgr::getSingleton()->getCommand(IntentoToCommandName[m_intentions.front()], &id);
		cmd->init(m_entity, (void*)CommandExeType::AtOnce);
		CommandMgr::getSingleton()->addCommand(cmd);
		m_intentions.pop();
		return true;
	}
}


void Brain::process(const float dt)
{
	m_timer += dt;
	if (m_timer >= m_exploreTime)
	{
		m_intentions = std::queue<Intention::Enum>();
		m_intentions.push(Intention::Explore);
		executeAction();

		typedef Graph_SearchAStar<Graph, Heuristic_Dijkstra> AStar;
		auto search = AStar(m_exploSparseGraph, PhysicMgr::getSingleton()->getCase(m_entity)->index, LevelMgr::getSingleton()->getHigherScoreCase()->index);
		createIntention(search.GetPathToTarget());

		if (m_intentions.empty())
		{
			m_exploreTime += m_exploreTime;
		}
		else
		{
			m_exploreTime = std::max(m_exploreTime - 1.0f, 1.0f);
		}
	}
	else
	{
		executeAction();
	}
}

void Brain::explore()
{
	m_exploSparseGraph.update();
}

void Brain::initGraph()
{
	m_cacheGrid = LevelMgr::getSingleton()->getGrid();
	for (auto& line : m_cacheGrid)
	{
		for (auto& caseHandler : line)
		{
			Node node;
			node.caseHandler = caseHandler;
			m_exploSparseGraph.AddNode(node);
		}
	}
	
	for (int i = 0; i < (int)m_cacheGrid.size(); i++)
	{
		for (int j = 0; j < (int)m_cacheGrid[i].size(); j++)
		{
			Node node;
			node.caseHandler = m_cacheGrid[i][j];
			GraphEdge edge;
			edge.SetFrom(node.Index());

			if (i == 0)
			{
				if (j == 0)
				{
					node.caseHandler = m_cacheGrid[i][j + 1];
					edge.SetTo(node.Index());
					m_exploSparseGraph.AddEdge(edge);
					node.caseHandler = m_cacheGrid[i + 1][j];
					edge.SetTo(node.Index());
					m_exploSparseGraph.AddEdge(edge);
				}
				else if (j == m_cacheGrid[i].size() - 1)
				{
					node.caseHandler = m_cacheGrid[i][j - 1];
					edge.SetTo(node.Index());
					m_exploSparseGraph.AddEdge(edge);
					node.caseHandler = m_cacheGrid[i + 1][j];
					edge.SetTo(node.Index());
					m_exploSparseGraph.AddEdge(edge);
				}
				else
				{
					node.caseHandler = m_cacheGrid[i][j - 1];
					edge.SetTo(node.Index());
					m_exploSparseGraph.AddEdge(edge);
					node.caseHandler = m_cacheGrid[i][j + 1];
					edge.SetTo(node.Index());
					m_exploSparseGraph.AddEdge(edge);
					node.caseHandler = m_cacheGrid[i + 1][j];
					edge.SetTo(node.Index());
					m_exploSparseGraph.AddEdge(edge);
				}
			}
			else if (i == m_cacheGrid.size() - 1)
			{
				if (j == 0)
				{
					node.caseHandler = m_cacheGrid[i][j + 1];
					edge.SetTo(node.Index());
					m_exploSparseGraph.AddEdge(edge);
					node.caseHandler = m_cacheGrid[i - 1][j];
					edge.SetTo(node.Index());
					m_exploSparseGraph.AddEdge(edge);
				}
				else if (j == m_cacheGrid[i].size() - 1)
				{
					node.caseHandler = m_cacheGrid[i][j - 1];
					edge.SetTo(node.Index());
					m_exploSparseGraph.AddEdge(edge);
					node.caseHandler = m_cacheGrid[i - 1][j];
					edge.SetTo(node.Index());
					m_exploSparseGraph.AddEdge(edge);
				}
				else
				{
					node.caseHandler = m_cacheGrid[i][j - 1];
					edge.SetTo(node.Index());
					m_exploSparseGraph.AddEdge(edge);
					node.caseHandler = m_cacheGrid[i][j + 1];
					edge.SetTo(node.Index());
					m_exploSparseGraph.AddEdge(edge);
					node.caseHandler = m_cacheGrid[i - 1][j];
					edge.SetTo(node.Index());
					m_exploSparseGraph.AddEdge(edge);
				}
			}
			else if (j == 0)
			{
				node.caseHandler = m_cacheGrid[i][j + 1];
				edge.SetTo(node.Index());
				m_exploSparseGraph.AddEdge(edge);
				node.caseHandler = m_cacheGrid[i - 1][j];
				edge.SetTo(node.Index());
				m_exploSparseGraph.AddEdge(edge);
				node.caseHandler = m_cacheGrid[i + 1][j];
				edge.SetTo(node.Index());
				m_exploSparseGraph.AddEdge(edge);
			}
			else if (j == m_cacheGrid[i].size() - 1)
			{
				node.caseHandler = m_cacheGrid[i][j - 1];
				edge.SetTo(node.Index());
				m_exploSparseGraph.AddEdge(edge);
				node.caseHandler = m_cacheGrid[i - 1][j];
				edge.SetTo(node.Index());
				m_exploSparseGraph.AddEdge(edge);
				node.caseHandler = m_cacheGrid[i + 1][j];
				edge.SetTo(node.Index());
				m_exploSparseGraph.AddEdge(edge);
			}
			else
			{
				node.caseHandler = m_cacheGrid[i][j - 1];
				edge.SetTo(node.Index());
				m_exploSparseGraph.AddEdge(edge);
				node.caseHandler = m_cacheGrid[i][j + 1];
				edge.SetTo(node.Index());
				m_exploSparseGraph.AddEdge(edge);
				node.caseHandler = m_cacheGrid[i - 1][j];
				edge.SetTo(node.Index());
				m_exploSparseGraph.AddEdge(edge);
				node.caseHandler = m_cacheGrid[i + 1][j];
				edge.SetTo(node.Index());
				m_exploSparseGraph.AddEdge(edge);
			}
		}
	}
}

void Brain::createIntention(std::list<int> path)
{
	for (auto& nodeID : path)
	{
		auto node = m_exploSparseGraph.GetNode(nodeID);
		auto casePos = node.cacheCase.background->getPosition();
		auto entPos = m_entity->getPosition();
	
		if (casePos.x < entPos.x)
		{
			m_intentions.push(Intention::MoveLeft);
		}
		else if (casePos.x > entPos.x)
		{
			m_intentions.push(Intention::MoveRight);
		}
		else if (casePos.y < entPos.y)
		{
			m_intentions.push(Intention::MoveUp);
		}
		else if (casePos.y > entPos.y)
		{
			m_intentions.push(Intention::MoveDown);
		}

		for (uint32_t i = 0; i < node.cacheCase.jewels; i++)
		{
			m_intentions.push(Intention::PickUp);
		}

		for (uint32_t i = 0; i < node.cacheCase.dusts; i++)
		{
			m_intentions.push(Intention::Clean);
		}
	}
}