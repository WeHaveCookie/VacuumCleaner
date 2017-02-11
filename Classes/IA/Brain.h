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

	const Vector2 Pos()
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

private:
	void initGraph();
	void createIntention(std::list<int> path);

	int m_energy;
	std::queue<Intention::Enum> m_intentions;
	float m_exploreTime;
	float m_timer;
	std::vector<std::vector<CaseHandler*>> m_cacheGrid;
	Entity*	m_entity;
	Graph m_exploSparseGraph;
};
