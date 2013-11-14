#pragma once
#include <BWAPI.h>

class CommanderAgent
{
public:
	CommanderAgent(void);
	~CommanderAgent(void);
	void onFrame(void);
	void createSupply(BWAPI::Unit u);
	int calculateBarracksFromCommandCenter(BWAPI::Unit u);
	void createBarrackNearCommandCenter(BWAPI::Unit u);

private:
	BWAPI::Unitset _commandCenters;
	BWAPI::Unitset _barracks;
};
