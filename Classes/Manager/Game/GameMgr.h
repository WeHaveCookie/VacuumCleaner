#pragma once

#include "Manager/Manager.h"

class Entity;

class GameMgr : public Manager
{
    public:
		static GameMgr* getSingleton() { return s_singleton; }
        GameMgr();
        ~GameMgr();

        // Function
		void init();
		void process(const float dt);
		void end();
		void paint();
		void showImGuiWindow(bool* window);

		const bool isRunning() const;
		void setFrameRate(uint32_t frameRate);
		sf::RenderWindow* getMainRenderWindow() const { return m_mainRenderWindow; }
		const sf::Time getProcessTime() const { return m_processTime; }
		const char* getGameName() const { return m_gameName; }
		const int getMovementSpeed() const { return m_movementSpeed; }
		Entity* getEntityPlayer(uint32_t id = 0);
		void setNumberPlayer(uint32_t nbr);

		const int getDustFactor() const { return m_dustFactor; }
		const int getJewelFactor() const { return m_jewelFactor; }

    protected:
    private:
		void generateEnvironment(const float dt);

        // Function
		static GameMgr*			s_singleton;
		sf::RenderWindow*		m_mainRenderWindow;
		sf::Time				m_processTime;
		int						m_framerate;
		char					m_gameName[128];
		int						m_movementSpeed;
		std::vector<uint32_t>	m_playersId;
		uint32_t				m_nbrPlayers;

		float					m_updateEnvironmentTimer;
		float					m_updateEnvironmentDelay;
		int						m_dustsPerSpawn;
		int						m_jewelsPerSpawn;
		int						m_dustFactor;
		int						m_jewelFactor;
};
