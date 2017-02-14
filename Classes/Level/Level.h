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
	Vector2		currentPos;
	int			index; 

	CaseHandler()
	{
		dusts = 0;
		jewels = 0;
		background = nullptr;
		currentPos = Vector2();
	}

	void pushEnt(Entity* ent);
	int clean();
	int pickJewels();
	void erase(uint32_t id);
	const uint32_t getScore() const;
	void cleanAll();
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
	const sf::Vector2f getSize() const { return m_size; }
	void createGrid(const char* path);
	const std::vector<std::vector<CaseHandler*>> getGrid();
	const std::vector<std::vector<CaseHandler>> getCacheGrid();
	void registrerIntoGrid(Entity* ent, sf::Vector2i pos);

	CaseHandler* getHigherScoreCase();
	CaseHandler* getCase(uint32_t id);
	const sf::Vector2f getCaseSize() const { return m_caseSize; }
	const sf::Vector2f getPosition() const { return m_position; }
	const sf::Vector2f getSizeLevel() const { return sf::Vector2f(m_size.x * m_caseSize.x, m_size.y * m_caseSize.y); }
	void clearAllCases();

	void cleanCase(Entity* ent);
	void pickCase(Entity* ent);

	const int getScore() const { return m_score; }
	const uint32_t getUsedEnergy() const { return m_usedEnergy; }
	void useEnergy(uint32_t nbr = 1) { m_usedEnergy += nbr; }

	const int getJewelsLost() const { return m_jewelsLost; }
	const int getJewelsPick() const { return m_jewelsPick; }
	const int getSuckUpDusts() const { return m_suckUpDusts; }

private:

	std::string					m_path;
	std::string					m_name;
	sf::Vector2f				m_size;
	sf::Vector2f				m_caseSize;
	sf::Vector2f				m_position;
	std::vector<Entity*>		m_entitys;
	std::vector<Background*>	m_backgrounds;
	std::vector<Background*>	m_foregrounds;
	std::vector<std::vector<CaseHandler*>> m_grid;
	int					m_score;
	uint32_t			m_usedEnergy;
	int					m_jewelsLost;
	int					m_suckUpDusts;
	int					m_jewelsPick;
};

