#include "stdafx.h"
#include "LevelMgr.h"
#include "Level/Quadtree.h"
#include "Manager/Game/GameMgr.h"
#include "Manager/Render/RenderMgr.h"
#include "Level/Level.h"
#include "Manager/File/FileMgr.h"
#include "Utils/wcharUtils.h"
#include "Utils/Random.h"
#include "Manager/Entity/EntityMgr.h"

LevelMgr* LevelMgr::s_singleton = NULL;

LevelMgr::LevelMgr()
:Manager(ManagerType::Enum::Level)
{
	s_singleton = this;
	m_queryCount = 0.0f;
}

LevelMgr::~LevelMgr()
{
	delete m_quadtree;
}

void LevelMgr::init()
{
	m_processTime = sf::Time::Zero;
	m_quadtree = new Quadtree();
	m_quadtree->init(0.0f, 0.0f, 1920.0f, 1080.0f);
	m_quadtree->setNodeCapacity(50);
	m_level = new Level();
	loadLevel("Data/Level/level1.json");
	GameMgr::getSingleton()->setNumberPlayer(1);
}

void LevelMgr::process(const float dt)
{
	sf::Clock clock;
	m_quadtree->update();
	registerAsyncEnvironment();
	m_processTime = clock.getElapsedTime();
}

void LevelMgr::end()
{
}

void LevelMgr::paint()
{
	m_level->paint();
	//m_quadtree->paint();
}

void LevelMgr::showImGuiWindow(bool* window)
{
	if (ImGui::Begin("LevelMgr", window))
	{
		if(ImGui::CollapsingHeader("Quadtree"))
		{
			auto elements = m_quadtree->getAllElements();
			for (auto& element : elements)
			{
				ImGui::Text("%i - %s", element->getUID(), element->getName());
				if (ImGui::IsItemClicked())
				{
					element->showInfo();
				}
			}
		}

		ImGui::Text("Current Level : %s", m_level->getName());
		ImGui::Text("x = %f; y = %f", m_level->getSize().x, m_level->getSize().y);
		
		std::vector<std::wstring> files;
		FileMgr::GetFilesInDirectory(files, L"Data/Level", L".json");

		char** filesLabel = (char**)malloc(sizeof(char*) * files.size());

		for (unsigned int i = 0; i < files.size(); i++)
		{
			filesLabel[i] = (char*)malloc(sizeof(char) * files[i].size() + 1); // +1 for null terminated
			strcpy(filesLabel[i], WideChartoAscii(files[i]).c_str());
		}

		static int itemID = 0;

		ImGui::Combo("Level", &itemID, (const char**)filesLabel, files.size());
		ImGui::SameLine();
		if (ImGui::Button("Load"))
		{
			unloadLevel();
			loadLevel(filesLabel[itemID]);
		}
		ImGui::SameLine();
		if (ImGui::Button("Unload"))
		{
			unloadLevel();
		}

		for (unsigned int i = 0; i < files.size(); i++)
		{
			free(filesLabel[i]);
		}
		free(filesLabel);

		auto ent = getHigherScoreCase();
		if (ent != nullptr)
		{
			ImGui::Text("Higher Score : %i", ent->getScore());
			if (ImGui::IsItemClicked())
			{
				ent->background->showInfo();
			}
		}
		if (ImGui::Button("clean All Cases"))
		{
			m_level->clearAllCases();
		}

		ImGui::Text("Score : %i - Energy : %i", m_level->getScore(), getUsedEnergy());
		ImGui::Text("Jewels lost : %i", m_level->getJewelsLost());
		ImGui::Text("Suck up dust : %i", m_level->getSuckUpDusts());
		ImGui::Text("Jewels pick : %i", m_level->getJewelsPick());
	}
	ImGui::End();
}

void LevelMgr::registerEntity(Entity* ent)
{
	m_quadtree->registerEntity(ent);
}

void LevelMgr::unregisterEntity(uint32_t id)
{
	m_quadtree->unregisterEntity(id);
}

std::vector<Entity*> LevelMgr::getEntityAround(Entity* ent, sf::FloatRect bound, EntityType::Enum type)
{
	m_queryCount++;
	return m_quadtree->queryRange(ent, bound, type);
}

int LevelMgr::getRegisterCount() 
{ 
	return m_quadtree->getRegisterCount(); 
}

int LevelMgr::getUnregisterCount() 
{ 
	return m_quadtree->getUnregisterCount(); 
}

int LevelMgr::getMasterRegisterCount()
{
	return m_quadtree->getMasterRegisterCount();
}

int LevelMgr::getQueryCount()
{
	return m_quadtree->getQueryCount();
}

bool LevelMgr::loadLevel(char* path)
{
	return m_level->load(path);
}

uint32_t LevelMgr::loadLevelAsync()
{
	return 1;
}

void LevelMgr::unloadLevel()
{
	m_level->unload();
}

void LevelMgr::createAsyncDusts(uint32_t nbr)
{
	for (uint32_t i = 0; i < nbr; i++)
	{
		m_loadingEnvironment.push_back(EntityMgr::getSingleton()->createAsyncEntity("Data/Environment/dust.json"));
	}
}

void LevelMgr::createAsyncJewels(uint32_t nbr)
{
	for (uint32_t i = 0; i < nbr; i++)
	{
		m_loadingEnvironment.push_back(EntityMgr::getSingleton()->createAsyncEntity("Data/Environment/jewel.json"));
	}
}

CaseHandler* LevelMgr::getHigherScoreCase()
{
	return m_level->getHigherScoreCase();
}

void LevelMgr::cleanCase(Entity* ent)
{
	m_level->cleanCase(ent);
}

void LevelMgr::pickCase(Entity* ent)
{
	m_level->pickCase(ent);
}

void LevelMgr::registerAsyncEnvironment()
{
	std::list<uint32_t> environmentOnLoading;
	for (auto& taskID : m_loadingEnvironment)
	{
		auto ent = EntityMgr::getSingleton()->getAsyncEntity(taskID);
		if (ent != nullptr)
		{
			sf::Vector2i pos(randIntBorned(0, m_level->getSize().x), randIntBorned(0, m_level->getSize().y));
			m_level->registrerIntoGrid(ent, pos);
		}
		else
		{
			environmentOnLoading.push_back(taskID);
		}
	}
	m_loadingEnvironment = environmentOnLoading;
}