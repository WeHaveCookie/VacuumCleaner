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
	m_exploSparseGraph(false),
	m_debugPause(false),
	m_displayInfo(true)
{
	m_energy = 1;
	m_exploreTime = 10.0f;
	m_timer = 0.0f;
	m_currentIntention = 0;
	m_timeToFinishWork = 10.0f;
	initGraph();
}

Brain::~Brain()
{
}

const bool Brain::executeAction()
{

	if (m_intentions.empty()) {
		m_currentIntention++;
		if (m_timeToFinishWork > m_timer)
		{
			m_timeToFinishWork = m_timer;
		}
		return false;
	}
	else {
		if (m_entity->hasTarget())
		{
			return true;
		}
		int id;
		Command* cmd = CommandMgr::getSingleton()->getCommand(IntentoToCommandName[m_intentions.front()], &id);
		cmd->init(m_entity);
		cmd->setExeType(CommandExeType::AtOnce);
		CommandMgr::getSingleton()->addCommand(cmd);
		m_intentions.pop();
		m_currentIntention++;
		return true;
	}
}


void Brain::process(const float dt)
{
	if (m_debugPause)
	{
		return;
	}
	m_timer += dt;
	if (m_timer >= m_exploreTime)
	{
		int id;
		Command* cmd = CommandMgr::getSingleton()->getCommand("CommandExplore", &id);
		cmd->init(m_entity);
		cmd->setExeType(CommandExeType::AtOnce);
		CommandMgr::getSingleton()->addCommand(cmd);
	}
	executeAction();
}

void Brain::explore()
{
	if (!m_intentions.empty())
	{
		m_exploreTime += m_exploreTime;
	}
	else
	{
		m_exploreTime = std::max(m_exploreTime - ((m_exploreTime - m_timeToFinishWork) / 2.0f), 1.0f);
	}
	m_intentions = std::queue<Intention::Enum>();
	m_debugIntentions.clear();
	m_exploSparseGraph.update();
	m_grid = LevelMgr::getSingleton()->getCacheGrid();
	typedef Graph_SearchAStar<Graph, Heuristic_Dijkstra> AStar;
	auto bestCase = getHigherScoreCase();
	if (bestCase >= 0)
	{
		auto search = AStar(m_exploSparseGraph, PhysicMgr::getSingleton()->getCase(m_entity)->index, bestCase);
		createIntention(search.GetPathToTarget());
	}
	m_currentIntention = -1;
	m_timer = 0.0f;
	m_timeToFinishWork = m_exploreTime;
}

void Brain::initGraph()
{
	auto cacheGrid = LevelMgr::getSingleton()->getGrid();
	
	for (int i = 0; i < (int)cacheGrid.size(); i++)
	{
		for (int j = 0; j < (int)cacheGrid[i].size(); j++)
		{
			Node node;
			node.caseHandler = cacheGrid[i][j];
			m_exploSparseGraph.AddNode(node);
			GraphEdge edge;
			edge.SetFrom(node.Index());

			if (i == 0 && j == 0) continue;
			if (i == 0)
			{
				node.caseHandler = cacheGrid[i][j - 1];
				edge.SetTo(node.Index());
				m_exploSparseGraph.AddEdge(edge);
			}
			else if (j == 0)
			{
				node.caseHandler = cacheGrid[i - 1][j];
				edge.SetTo(node.Index());
				m_exploSparseGraph.AddEdge(edge);
			}
			else
			{
				node.caseHandler = cacheGrid[i][j - 1];
				edge.SetTo(node.Index());
				m_exploSparseGraph.AddEdge(edge);
				node.caseHandler = cacheGrid[i - 1][j];
				edge.SetTo(node.Index());
				m_exploSparseGraph.AddEdge(edge);
			}
		}
	}
}

void Brain::createIntention(std::list<int> path)
{
	auto currentPos = PhysicMgr::getSingleton()->getCase(m_entity)->background->getPosition();
	for (auto& nodeID : path)
	{
		auto node = m_exploSparseGraph.GetNode(nodeID);
		auto casePos = node.cacheCase.background->getPosition();
		
		if (casePos.x < currentPos.x)
		{
			m_intentions.push(Intention::MoveLeft);
			m_debugIntentions.push_back(Intention::MoveLeft);
		}
		else if (casePos.x > currentPos.x)
		{
			m_intentions.push(Intention::MoveRight);
			m_debugIntentions.push_back(Intention::MoveRight);
		}
		else if (casePos.y < currentPos.y)
		{
			m_intentions.push(Intention::MoveUp);
			m_debugIntentions.push_back(Intention::MoveUp);
		}
		else if (casePos.y > currentPos.y)
		{
			m_intentions.push(Intention::MoveDown);
			m_debugIntentions.push_back(Intention::MoveDown);
		}

		for (uint32_t i = 0; i < node.cacheCase.jewels; i++)
		{
			m_intentions.push(Intention::PickUp);
			m_debugIntentions.push_back(Intention::PickUp);
		}

		for (uint32_t i = 0; i < node.cacheCase.dusts; i++)
		{
			m_intentions.push(Intention::Clean);
			m_debugIntentions.push_back(Intention::Clean);
		}
		currentPos = casePos;
	}
}

void Brain::showImGuiWindow()
{
	if (m_displayInfo)
	{
		std::string name = "Brain : " + std::to_string(m_entity->getUID());
		if (ImGui::Begin(name.c_str(), &m_displayInfo))
		{
			ImGui::Checkbox("Pause", &m_debugPause);
			ImGui::Text("Best cache case : %i", getHigherScoreCase());
			auto colorRatio = (m_timer / m_exploreTime);
			auto r = 126.0f + 37.0f * colorRatio;
			auto g = 253.0f - 246.0f * colorRatio;
			auto b = 20.0f - 20.0f * colorRatio;
			float sc = 1.0f / 255.0f;
			auto color = ImColor(r*sc, g*sc, b*sc);
			ImGui::TextColored(color, "Timer %f / %f", m_timer, m_exploreTime);
			ImGui::Text("Work Done in %f sec", m_timeToFinishWork);
			ImGui::Text("Intention - nbr : %i", m_intentions.size());
			ImGui::Separator();
			int counter = 0;
			for (auto& intention : m_debugIntentions)
			{
				if (counter < m_currentIntention)
				{
					ImGui::TextColored(ImColor(0, 0, 0), IntentoToCommandName[intention]);
				}
				else if (counter == m_currentIntention)
				{
					ImGui::TextColored(ImColor(128, 255, 0), IntentoToCommandName[intention]);
				}
				else
				{
					ImGui::TextColored(ImColor(0, 153, 0), IntentoToCommandName[intention]);
				}
				counter++;
			}
		}
		ImGui::End();
	}
}

int Brain::getHigherScoreCase()
{
	uint32_t score = 0;
	int ans = -1;
	for (auto& line : m_grid)
	{
		for (auto& handler : line)
		{
			if (handler.getScore() > score)
			{
				score = handler.getScore();
				ans = handler.index;
			}
		}
	}
	return ans;
}