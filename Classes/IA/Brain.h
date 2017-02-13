#pragma once
#include "Level/Level.h"
#include "Graph/SparseGraph.h"
#include "Graph/GraphEdgeTypes.h"
#include "Entity/Entity.h"

namespace Intention
{
	enum Enum
	{
		MoveUp,
		MoveRight,
		MoveDown,
		MoveLeft,
		Clean,
		PickUp,
		Explore
	};
}

class Entity;

struct Node
{
	CaseHandler*	caseHandler;
	CaseHandler		cacheCase;

	void update()
	{
		cacheCase = *caseHandler;
	}

	int Index()
	{
		return caseHandler->index;
	}

	const Vector2 Pos() const
	{
		return cacheCase.background->getPosition();
	}

};

typedef SparseGraph<Node, GraphEdge> Graph;

class Brain
{

public:
	Brain(Entity* ent);
	~Brain();

	const bool executeAction();
	void process(const float dt);
	void explore();
	void showImGuiWindow();

	void showInfo() { m_displayInfo = !m_displayInfo; }

private:
	void initGraph();
	void createIntention(std::list<int> path);
	int getHigherScoreCase();

	bool						m_displayInfo;
	int							m_energy;
	int							m_currentIntention;
	std::vector<Intention::Enum> m_debugIntentions;
	float						m_exploreTime;
	float						m_timer;
	float						m_timeToFinishWork;
	Entity*						m_entity;
	Graph						m_exploSparseGraph;
	bool						m_debugPause;

	//Les intentions de l'agent
	std::queue<Intention::Enum>	m_intentions;

	//Croyance de l'agent == Etat de l'environnement
	std::vector<std::vector<CaseHandler>>	m_croyance;
};
