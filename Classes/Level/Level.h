#pragma once

class Entity;

struct Background {
	sf::Texture	m_texture;
	sf::Sprite	m_sprite;
	int			m_backgroundLevel;

	void paint();
};

struct CaseHandler
{
	std::vector<uint32_t> entities;
	uint32_t	dusts;
	uint32_t	jewels;
	Entity*		background;
	
	void pushEnt(Entity* ent);
	void clean();
	void cleanJewels();
};

class Level
{

	public:
		Level();
		~Level();

		void paint();
		bool load(const char* path);
		bool reload();
		void unload();

		const char* getName() const { return m_name.c_str(); }
		const sf::Vector2f getSize() const{ return m_size; }
		void createGrid(const char* path);
		const std::map<std::pair<uint32_t, uint32_t>, CaseHandler> getGrid() const;
		void registrerIntoGrid(Entity* ent, sf::Vector2i pos);
		void cleanCase(sf::Vector2i pos);
		void removeJewels(sf::Vector2i pos);

	private:

		std::string					m_path;
		std::string					m_name;
		sf::Vector2f				m_size;
		sf::Vector2f				m_caseSize;
		std::vector<Entity*>		m_entitys;
		std::vector<Background*>	m_backgrounds;
		std::vector<Background*>	m_foregrounds;
		std::map<std::pair<uint32_t,uint32_t>, CaseHandler > m_grid;
};

