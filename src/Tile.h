#pragma once
#ifndef __TILE__
#define __TILE__

#include "NavigationObject.h"
#include "NeighbourTile.h"
#include "Label.h"
#include "TileStatus.h"

class Tile : public NavigationObject
{
public:
	// Constructor
	Tile();
	
	// Destructor
	~Tile();
	
	// Life-Cycle Functions
	void draw() override;
	void update() override;
	void clean() override;

	Tile* getNeighbourTile(NeighbourTile position);
	void setNeighbourTile(NeighbourTile position, Tile* tile);
	
	float getTileCost();
	void setTileCost(float cost);

	const TileStatus& getTileStatus() const;
	void setTileStatus(const TileStatus& status);

	void addLabels();
	void setLabelsEnabled(bool state);

private:
	float m_cost;
	TileStatus m_status;

	Label* m_costLabel;
	Label* m_statusLabel;

	Tile* m_neighbours[NUM_OF_NEIGHBOUR_TILES];
};

#endif /* defined (__TILE__) */