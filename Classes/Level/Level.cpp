#include "stdafx.h"
#include "Level.h"
#include "Entity/Entity.h"
#include "Manager/Render/RenderMgr.h"
#include "Manager/File/FileMgr.h"
#include "../../External/rapidjson/document.h"
#include "Utils/jsonUtils.h"
#include "Manager/Entity/EntityMgr.h"
#include "Utils/containerUtils.h"
#include "Manager/Game/GameMgr.h"

void Background::paint()
{
	auto rdrWin = RenderMgr::getSingleton()->getMainRenderWindow();
	rdrWin->draw(m_sprite);
}


void CaseHandler::pushEnt(Entity* ent)
{
	entities.push_back(ent->getUID());
	if (ent->getElement() == EntityElement::Dust)
	{
		dusts++;
	}
	else if (ent->getElement() == EntityElement::Jewel)
	{
		jewels++;
	}
	ent->setPosition(currentPos);
	currentPos.x = ent->getPosition().x + ent->getGlobalBounds().width;
	if (currentPos.x > background->getPosition().x + (background->getGlobalBounds().width - ent->getGlobalBounds().width))
	{
		currentPos.y += ent->getGlobalBounds().height;
		if (currentPos.y > background->getPosition().y + (background->getGlobalBounds().height - ent->getGlobalBounds().height))
		{
			currentPos = Vector2(0.0, 0.0);
		}
		currentPos.x = background->getPosition().x;
	}
}

void CaseHandler::erase(uint32_t id)
{
	auto it = entities.begin();
	for (auto& ent : entities)
	{
		if (ent == id)
		{
			entities.erase(it);
			break;
		}
		it++;
	}
}

void CaseHandler::clean()
{
	std::vector < uint32_t > deleteEntity;
	bool dustRemove = false;
	for (auto& ent : entities)
	{
		if (EntityMgr::getSingleton()->getEntity(ent)->getElement() == EntityElement::Dust && !dustRemove)
		{
			dustRemove = true;
			EntityMgr::getSingleton()->deleteEntity(ent);
			deleteEntity.push_back(ent);
			dusts--;
		}
		
		if (EntityMgr::getSingleton()->getEntity(ent)->getElement() != EntityElement::Dust)
		{
			EntityMgr::getSingleton()->deleteEntity(ent);
			deleteEntity.push_back(ent);
			jewels--;
		}
	}
	
	for (auto& del : deleteEntity)
	{
		erase(del);
	}
}

void CaseHandler::cleanJewels()
{
	for (auto& ent : entities)
	{
		if (EntityMgr::getSingleton()->getEntity(ent)->getElement() == EntityElement::Jewel)
		{
			EntityMgr::getSingleton()->deleteEntity(ent);
		}
	}
	jewels = 0;
}

const uint32_t CaseHandler::getScore() const
{
	return dusts * GameMgr::getSingleton()->getDustFactor() + jewels * GameMgr::getSingleton()->getJewelFactor();
}

Level::Level()
{
}


Level::~Level()
{
}

void Level::paint()
{
	for (auto &background : m_backgrounds)
	{
		background->paint();
	}
}

bool cmpDisplayLevelLTH(Background* a, Background* b)
{
	return a->m_backgroundLevel >= b->m_backgroundLevel;

}

bool Level::load(const char* path)
{
	m_path = path;

	char* json;
	int jsonSize;

	FileMgr::ReadFile(path, (void**)&json, &jsonSize);

	json[jsonSize] = 0;

	rapidjson::Document document;
	document.Parse((char*)json);
	auto error = document.HasParseError();
	auto object = document.IsObject();
	if (error || !object)
	{
		return false;
	}
	assert(!error);
	assert(object);

	auto namePtr = &m_name;
	checkAndAffect(&document, "Name", ValueType::String, (void**)&namePtr, "NoName", sizeof("NoName"));

	auto sizePtr = &m_size;
	checkAndAffect(&document, "Size", ValueType::Vector2, (void**)&sizePtr);

	auto caseSizePtr = &m_caseSize;
	checkAndAffect(&document, "CaseSize", ValueType::Vector2, (void**)&caseSizePtr);

	auto positionPtr = &m_position;
	checkAndAffect(&document, "Position", ValueType::Vector2, (void**)&positionPtr);

	if (document.HasMember("Path"))
	{
		createGrid(document["Path"].GetString());
	}

	if (document.HasMember("Backgrounds"))
	{
		const rapidjson::Value& backgrounds = document["Backgrounds"];

		for (auto& background : backgrounds.GetArray())
		{
			
			assert(background.HasMember("Path") && background.HasMember("BackgroundLevel"));
			Background* back = new Background();
			auto path = background["Path"].GetString();
			auto load = back->m_texture.loadFromFile(path);
			back->m_sprite.setTexture(back->m_texture);
			back->m_backgroundLevel = background["BackgroundLevel"].GetUint();

			if (background.HasMember("Position") && background["Position"].GetArray().Size())
			{
				back->m_sprite.setPosition(sf::Vector2f(background["Position"][0].GetFloat(), background["Position"][1].GetFloat()));
			}

			pushSorted(&m_backgrounds, back, cmpDisplayLevelLTH);
		}
	}

	if (document.HasMember("Entitys"))
	{
		const rapidjson::Value& entitys = document["Entitys"];

		for (auto& entity : entitys.GetArray())
		{
			assert(entity.HasMember("Path"));
			
			
			if (entity.HasMember("Position"))
			{
				const rapidjson::Value& clones = entity["Position"];

				for (auto& clone : clones.GetArray())
				{
					Entity* ent = EntityMgr::getSingleton()->createEntity(entity["Path"].GetString());
					ent->setPosition(Vector2(clone[0].GetFloat(), clone[1].GetFloat()));
					m_entitys.push_back(ent);
				}
			}
			else
			{
				Entity* ent = EntityMgr::getSingleton()->createEntity(entity["Path"].GetString());
				m_entitys.push_back(ent);
			}
		}
	}

	FileMgr::CloseFile(json);
	return true;
}

bool Level::reload()
{
	unload();
	return load(m_path.c_str());
}

void Level::unload()
{
	m_backgrounds.clear();
	for (auto &entity : m_entitys)
	{
		EntityMgr::getSingleton()->deleteEntity(entity->getUID());
	}
	m_entitys.clear();
	m_name = "";
	m_size = sf::Vector2f();
}

void Level::createGrid(const char* path)
{
	m_grid.clear();
	for (int i = 0; i < m_size.x; i++)
	{
		std::vector<CaseHandler*> line;
		for (int j = 0; j < m_size.y; j++)
		{
			auto back = EntityMgr::getSingleton()->createEntity(path);
			back->setPosition(Vector2(m_caseSize.x * i + m_position.x, m_caseSize.y * j + m_position.y));
			CaseHandler* cHandler = new CaseHandler();
			cHandler->dusts = 0;
			cHandler->jewels = 0;
			cHandler->background = back;
			cHandler->currentPos = back->getPosition();
			line.push_back(cHandler);
			back->setCaseHandler(cHandler);
		}
		m_grid.push_back(line);
	}
}

const std::vector<std::vector<CaseHandler*>> Level::getGrid() const
{
	return m_grid;
}

void Level::registrerIntoGrid(Entity* ent, sf::Vector2i pos)
{
	m_grid[pos.x][pos.y]->pushEnt(ent);
}

void Level::cleanCase(sf::Vector2i pos)
{
	m_grid[pos.x][pos.y]->clean();
}

void Level::removeJewels(sf::Vector2i pos)
{
	m_grid[pos.x][pos.y]->cleanJewels();
}

CaseHandler* Level::getHigherScoreCase()
{
	uint32_t score = 0;
	CaseHandler* ans = nullptr;
	for (auto& line : m_grid)
	{
		for (auto& handler : line)
		{
			if (handler->getScore() > score)
			{
				score = handler->getScore();
				ans = handler;
			}
		}
	}
	return ans;
}
