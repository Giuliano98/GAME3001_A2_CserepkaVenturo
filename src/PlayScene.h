#pragma once
#ifndef __PLAY_SCENE__
#define __PLAY_SCENE__

#include "Scene.h"
#include "Button.h"
#include "Label.h"
#include "Obstacle.h"
#include "ship.h"
#include "SpaceShip.h"
#include "Target.h"
#include "Tile.h"
#include "Heuristic.h"

class PlayScene : public Scene
{
public:
	PlayScene();
	~PlayScene();

	// Scene LifeCycle Functions
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;
	virtual void handleEvents() override;
	virtual void start() override;

private:
	// IMGUI Function
	void GUI_Function();
	std::string m_guiTitle;
	glm::vec2 m_mousePosition;
	bool m_isGridEnabled;

	Target* m_pTarget;
	Ship* m_pShip;
	Tile* m_pTile;

	// Pathfinding functions and objects
	void m_buildGrid();
	void m_computeTileCosts();
	void m_findShortestPath(); // Our path finding method.
	void m_displayPathList();

	const bool m_getGridEnabled() const;
	void m_setGridEnabled(bool state);
	std::vector<Tile*> m_pGrid;

	// convience functions
	Tile* m_getTile(int col, int row);
	Tile* m_getTile(glm::vec2 grid_position);

	// heuristic
	Heuristic m_currentHeuristic;

	// open, closed, and path lists
	std::vector<Tile*> m_pOpenList;
	std::vector<Tile*> m_pClosedList;
	std::vector<Tile*> m_pPathList;
	
	void m_moveShip();
	int m_moveCounter = 0;
	bool m_shipIsMoving;
};

#endif /* defined (__PLAY_SCENE__) */