#include "stdafx.h"
#include "Level.h"
#include "Entity/Entity.h"
#include "Manager/Render/RenderMgr.h"
#include "Manager/File/FileMgr.h"
#include "../../External/rapidjson/document.h"
#include "Utils/jsonUtils.h"
#include "Manager/Entity/EntityMgr.h"
#include "Utils/containerUtils.h"

void Background::paint()
{
	auto rdrWin = RenderMgr::getSingleton()->getMainRenderWindow();
	rdrWin->draw(m_sprite);
}


void CaseHandler::pushEnt(Entity* ent)
{
	entities.push_back(ent->getUID());
	if (ent->getElement() == EntityElement::dust)
	{
		dusts++;
	}
	else if (ent->getElement() == EntityElement::jewel)
	{
		jewels++;
	}
}

void CaseHandler::clean()
{
	bool dustRemove = false;
	for (auto& ent : entities)
	{
		if (EntityMgr::getSingleton()->getEntity(ent)->getElement() == EntityElement::dust && !dustRemove)
		{
			dustRemove = true;
			EntityMgr::getSingleton()->deleteEntity(ent);
		}
		
		if (EntityMgr::getSingleton()->getEntity(ent)->getElement() != EntityElement::dust)
		{
			EntityMgr::getSingleton()->deleteEntity(ent);
		}
		
	}
	entities.clear();
	dusts = 0;
	jewels = 0;
}

void CaseHandler::cleanJewels()
{
	for (auto& ent : entities)
	{
		if (EntityMgr::getSingleton()->getEntity(ent)->getElement() == EntityElement::jewel)
		{
			EntityMgr::getSingleton()->deleteEntity(ent);
		}
	}
	jewels = 0;
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
		for (int j = 0; j < m_size.y; j++)
		{
			auto back = EntityMgr::getSingleton()->createEntity(path);
			back->setPosition(Vector2(m_caseSize.x * i, m_caseSize.y * j));
			CaseHandler cHandler;
			cHandler.dusts = 0;
			cHandler.jewels = 0;
			cHandler.background = back;
			m_grid.at(std::make_pair(i, j)) = cHandler;
		}
	}
}

const std::map<std::pair<uint32_t, uint32_t>, CaseHandler> Level::getGrid() const
{
	return m_grid;
}

void Level::registrerIntoGrid(Entity* ent, sf::Vector2i pos)
{
	auto cases = m_grid.at(std::make_pair(pos.x,pos.y));
	cases.pushEnt(ent);
}

void Level::cleanCase(sf::Vector2i pos)
{
	auto cases = m_grid.at(std::make_pair(pos.x, pos.y));
	cases.clean();
}

void Level::removeJewels(sf::Vector2i pos)
{
	auto cases = m_grid.at(std::make_pair(pos.x, pos.y));
	cases.cleanJewels();
}