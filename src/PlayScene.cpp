#include "PlayScene.h"
#include "Game.h"
#include "EventManager.h"

// required for IMGUI
#include "imgui.h"
#include "imgui_sdl.h"
#include "Renderer.h"
#include "Util.h"

PlayScene::PlayScene() : m_moveCounter(0), m_shipIsMoving(false)
{
	PlayScene::start();
	// Set Background music
	SoundManager::Instance().load("../Assets/audio/BeepBox-Song.wav", "bg_music", SOUND_MUSIC);
	SoundManager::Instance().playMusic("bg_music", 10, 0);
	SoundManager::Instance().setMusicVolume(6);
}

PlayScene::~PlayScene()
= default;

void PlayScene::draw()
{
	drawDisplayList();
	
	if (EventManager::Instance().isKeyUp(SDL_SCANCODE_H))
	{
		GUI_Function();
	}
		
		TextureManager::Instance()->draw("asteroid", 420, 220, 0, 255, true);
		TextureManager::Instance()->draw("asteroid", 620, 180, 0, 255, true);
		TextureManager::Instance()->draw("asteroid", 620, 260, 0, 255, true);
		TextureManager::Instance()->draw("asteroid", 380, 180, 0, 255, true);
		TextureManager::Instance()->draw("asteroid", 500, 260, 0, 255, true);
		TextureManager::Instance()->draw("asteroid", 540, 220, 0, 255, true);

	/*if(EventManager::Instance().isIMGUIActive())
	{
		GUI_Function();	
	}*/

	SDL_SetRenderDrawColor(Renderer::Instance()->getRenderer(), 255, 255, 255, 255);
}

void PlayScene::update()
{
	updateDisplayList();
	if (m_shipIsMoving)
		m_moveShip();

	CollisionManager::AABBCheck(m_pShip, m_pTarget);
}

void PlayScene::clean()
{
	removeAllChildren();
}

void PlayScene::handleEvents()
{
	int x, y;
	EventManager::Instance().update();
	SDL_GetMouseState(&x, &y);

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_ESCAPE))
		TheGame::Instance()->quit();

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_1))
		TheGame::Instance()->changeSceneState(START_SCENE);

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_2))
		TheGame::Instance()->changeSceneState(END_SCENE);

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_F))
		m_findShortestPath();

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_M))
		m_shipIsMoving = true;

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_G))
		m_setGridEnabled(!m_getGridEnabled());

	if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		m_pShip->getTransform()->position.x = x;
		m_pShip->getTransform()->position.y = y;
	}
}

void PlayScene::start()
{

	// Set GUI Title
	m_guiTitle = "Play Scene";

	m_buildGrid();

	auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);

	m_currentHeuristic = EUCLIDEAN;
	
	// add the ship to the scene
	m_pShip = new Ship();
	m_pShip->getTransform()->position = m_getTile(1, 5)->getTransform()->position + offset;
	m_pShip->setGridPosition(1, 5);
	m_getTile(1, 5)->setTileStatus(START);
	addChild(m_pShip);

	// add the target to the scene
	m_pTarget = new Target();
	m_pTarget->getTransform()->position = m_getTile(15, 5)->getTransform()->position + offset;
	m_pTarget->setGridPosition(15, 5);
	m_getTile(15, 5)->setTileStatus(GOAL);
	addChild(m_pTarget);

	// Impassable objects.
	m_getTile(9, 4)->setTileStatus(IMPASSABLE);
	m_getTile(12, 6)->setTileStatus(IMPASSABLE);
	m_getTile(10, 5)->setTileStatus(IMPASSABLE);
	m_getTile(15, 4)->setTileStatus(IMPASSABLE);
	m_getTile(15, 6)->setTileStatus(IMPASSABLE);
	m_getTile(13, 5)->setTileStatus(IMPASSABLE);

	m_computeTileCosts();
}

void PlayScene::GUI_Function() 
{
	//TODO: We need to deal with this
	auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);

	// Always open with a NewFrame
	ImGui::NewFrame();

	// See examples by uncommenting the following - also look at imgui_demo.cpp in the IMGUI filter
	//ImGui::ShowDemoWindow();
	
	ImGui::Begin("GAME3001 - Lab Assignment 1 - CserepkaVenturo", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar);

	static bool isGridEnabled = false;
	if(ImGui::Checkbox("Grid Enabled", &isGridEnabled))
	{
		// toggle grid on/off
		m_setGridEnabled(isGridEnabled);
	}

	ImGui::Separator();

	auto radio = static_cast<int>(m_currentHeuristic);
	ImGui::LabelText("", "Heuristic Type");
	ImGui::RadioButton("Manhattan", &radio, static_cast<int>(MANHATTAN));
	ImGui::SameLine();
	ImGui::RadioButton("Euclidean", &radio, static_cast<int>(EUCLIDEAN));
	if (m_currentHeuristic != Heuristic(radio)) // Radio toggle
	{
		m_currentHeuristic = Heuristic(radio);
		m_computeTileCosts();
	}

	ImGui::Separator();

	static int startPosition[] = { m_pShip->getGridPosition().x, m_pShip->getGridPosition().y };
	if (ImGui::SliderInt2("Start Position", startPosition, 0, Config::COL_NUM - 1))
	{
		// Row adjustments
		if (startPosition[1] > Config::ROW_NUM - 1)
			startPosition[1] = Config::ROW_NUM - 1;

		SDL_RenderClear(Renderer::Instance()->getRenderer());
		// Reset previous tile status to UNVISITED.
		m_getTile(m_pShip->getGridPosition())->setTileStatus(UNVISITED);
		m_pShip->getTransform()->position = m_getTile(startPosition[0], startPosition[1])->getTransform()->position + offset;
		m_pShip->setGridPosition(startPosition[0], startPosition[1]);
		// Set tile status to START.
		m_getTile(m_pShip->getGridPosition())->setTileStatus(START);
		SDL_SetRenderDrawColor(Renderer::Instance()->getRenderer(), 255, 255, 255, 255);
		SDL_RenderPresent(Renderer::Instance()->getRenderer());
	}

	static int targetPosition[] = { m_pTarget->getGridPosition().x, m_pTarget->getGridPosition().y };
	if (ImGui::SliderInt2("Target Position", targetPosition, 0, Config::COL_NUM - 1))
	{
		// Row adjustments
		if (targetPosition[1] > Config::ROW_NUM - 1)
			targetPosition[1] = Config::ROW_NUM - 1;

		SDL_RenderClear(Renderer::Instance()->getRenderer());
		// Reset previous tile status to UNVISITED.
		m_getTile(m_pTarget->getGridPosition())->setTileStatus(UNVISITED);
		m_pTarget->getTransform()->position = m_getTile(targetPosition[0], targetPosition[1])->getTransform()->position + offset;
		m_pTarget->setGridPosition(targetPosition[0], targetPosition[1]);
		// Set tile status to GOAL.
		m_getTile(m_pTarget->getGridPosition())->setTileStatus(GOAL);
		m_computeTileCosts();
		SDL_SetRenderDrawColor(Renderer::Instance()->getRenderer(), 255, 255, 255, 255);
		SDL_RenderPresent(Renderer::Instance()->getRenderer());
	}
	
	ImGui::Separator();
	
	if (ImGui::Button("Find Shortest Path"))
	{
		m_findShortestPath();
	}

	if(ImGui::Button("Start"))
	{
		m_shipIsMoving = true;
	}

	ImGui::SameLine();
	
	if (ImGui::Button("Reset"))
	{

	}

	ImGui::Separator();
	
	ImGui::End();

	// Don't Remove this
	ImGui::Render();
	ImGuiSDL::Render(ImGui::GetDrawData());
	ImGui::StyleColorsDark();
}

void PlayScene::m_buildGrid()
{
	auto tileSize = Config::TILE_SIZE;
	
	// add tiles to the grid
	for (int row = 0; row < Config::ROW_NUM; ++row)
	{
		for (int col = 0; col < Config::COL_NUM; ++col)
		{
			Tile* tile = new Tile(); // create empty tile
			tile->getTransform()->position = glm::vec2(col * tileSize, row * tileSize);
			tile->setGridPosition(col, row);
			addChild(tile);
			tile->addLabels();
			tile->setEnabled(false);
			m_pGrid.push_back(tile);
		}
	}

	// create references for each tile to its neighbours
	for (int row = 0; row < Config::ROW_NUM; ++row)
	{
		for (int col = 0; col < Config::COL_NUM; ++col)
		{
			Tile* tile = m_getTile(col, row);

			// Topmost row
			if (row == 0)
			{
				tile->setNeighbourTile(TOP_TILE, nullptr);
			}
			else
			{
				tile->setNeighbourTile(TOP_TILE, m_getTile(col, row - 1));
			}

			// rightmost column
			if (col == Config::COL_NUM - 1)
			{
				tile->setNeighbourTile(RIGHT_TILE, nullptr);
			}
			else
			{
				tile->setNeighbourTile(RIGHT_TILE, m_getTile(col + 1, row));
			}

			// Bottommost row
			if (row == Config::ROW_NUM - 1)
			{
				tile->setNeighbourTile(BOTTOM_TILE, nullptr);
			}
			else
			{
				tile->setNeighbourTile(BOTTOM_TILE, m_getTile(col, row + 1));
			}

			// Leftmost column
			if (col == 0)
			{
				tile->setNeighbourTile(LEFT_TILE, nullptr);
			}
			else
			{
				tile->setNeighbourTile(LEFT_TILE, m_getTile(col - 1, row));
			}
		}
	}

	std::cout << m_pGrid.size() << std::endl;
}

void PlayScene::m_computeTileCosts()
{
	float distance = 0.0f, dx, dy;

	for (auto tile : m_pGrid)
	{
		switch (m_currentHeuristic)
		{
		case MANHATTAN:
			dx = abs(tile->getGridPosition().x - m_pTarget->getGridPosition().x);
			dy = abs(tile->getGridPosition().y - m_pTarget->getGridPosition().y);
			distance = dx + dy;
			break;
		case EUCLIDEAN:
			distance = Util::distance(m_pTarget->getGridPosition(), tile->getGridPosition());
			break;
		}
		tile->setTileCost(distance);
	}
}

void PlayScene::m_findShortestPath()
{
	// No way to reset...
	if (m_pPathList.empty())
	{
		// Step 1 - Add Start position to the open list
		auto startTile = m_getTile(m_pShip->getGridPosition());
		startTile->setTileStatus(OPEN);
		m_pOpenList.push_back(startTile);

		bool goalFound = false;

		// Step 2 - Loop until the OpenList is empty or the Goal is found
		while (!m_pOpenList.empty() && !goalFound)
		{
			auto min = INFINITY;
			Tile* minTile;
			int minTileIndex = 0;
			int count = 0;

			std::vector<Tile*> neighborList;
			for (int index = 0; index < NUM_OF_NEIGHBOUR_TILES; ++index)
			{
				neighborList.push_back(m_pOpenList[0]->getNeighbourTile(NeighbourTile(index)));
			}
			
			for (auto neighbour : neighborList)
			{
				if (neighbour != nullptr)
				{
					if (neighbour->getTileStatus() != GOAL) //  && neighbour->getTileStatus() != IMPASSABLE
					{
						if (neighbour->getTileCost() < min && neighbour->getTileStatus() !=	IMPASSABLE)
						{
							min = neighbour->getTileCost();
							minTile = neighbour;
							minTileIndex = count;
						}
						count++;
					}
					else
					{
						minTile = neighbour;
						m_pPathList.push_back(minTile);
						goalFound = true;
						break;
					}
				}
			}

			// remove the reference of the current tile in the open list
			m_pPathList.push_back(m_pOpenList[0]);
			m_pOpenList.pop_back(); // copies the open list

			// add the minTile to the openList
			m_pOpenList.push_back(minTile);
			minTile->setTileStatus(OPEN);
			neighborList.erase(neighborList.begin() + minTileIndex);

			// push all remaining neighbours onto the closed list
			for (auto neighbour : neighborList)
			{
				if (neighbour != nullptr)
				{
					if (neighbour->getTileStatus() == UNVISITED)
					{
						neighbour->setTileStatus(CLOSED);
						m_pClosedList.push_back(neighbour);
					}
				}
			}
		}
		m_displayPathList();
	}
}

void PlayScene::m_displayPathList()
{
	for (auto node : m_pPathList)
	{
		std::cout << "(" << node->getGridPosition().x << ", " << node->getGridPosition().y << ")" << std::endl;
	}
	std::cout << "Path Length: " << m_pPathList.size() << std::endl;
}

const bool PlayScene::m_getGridEnabled() const
{
	return m_isGridEnabled;
}

void PlayScene::m_setGridEnabled(bool state) 
{
	for (auto tile : m_pGrid)
	{
		tile->setEnabled(state);
		tile->setLabelsEnabled(state);
	}

	if(state == false)
	{
		SDL_RenderClear(Renderer::Instance()->getRenderer());
	}
}

Tile* PlayScene::m_getTile(const int col, const int row)
{
	return m_pGrid[(row * Config::COL_NUM) + col];
}

Tile* PlayScene::m_getTile(glm::vec2 grid_position)
{
	return m_pGrid[(grid_position.y * Config::COL_NUM) + grid_position.x];
}

void PlayScene::m_moveShip()
{
	auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);
	if (m_moveCounter < m_pPathList.size())
	{
		m_pShip->getTransform()->position = m_getTile(m_pPathList[m_moveCounter]->getGridPosition())->getTransform()->position + offset;
		m_pShip->setGridPosition(m_pPathList[m_moveCounter]->getGridPosition().x, m_pPathList[m_moveCounter]->getGridPosition().y);
		if (Game::Instance()->getFrames() % 20 == 0)
		{
			m_moveCounter++;
			SoundManager::Instance().playSound("move", 0);
		}
	}
	else
	{
		m_shipIsMoving = false;
	}
}