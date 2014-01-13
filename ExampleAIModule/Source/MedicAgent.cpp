#include <BWAPI.h>
#include <iostream>
#include <list>
#include <unordered_map>
#include <climits>
#include <deque>
//#include <random>

#include "MedicAgent.h"
#include "Task.h"
//#include "PositionTask.h"
#include "TaskAssociation.h"
#include "util.h"


using namespace BWAPI;
using namespace std;

MedicAgent::MedicAgent(Unit u) : gameUnit(u), state(NO_TASK), latencyFrames(10){
	lastPosition = Position(0,0);
	lastFrameCount = Broodwar->getFrameCount();
}

MedicAgent::~MedicAgent(void){
}

void MedicAgent::onFrame(unordered_map<TaskType, vector<Task>*> taskMap){
	//if is already engaged in task, continues it
	if(!gameUnit->isCompleted()){
		//|| Broodwar->getFrameCount() % latencyFrames != 0) {
		return;
	}

	int currentFrameCount = Broodwar->getFrameCount();
	if ( currentFrameCount >= lastFrameCount + 20){
		lastFrameCount = currentFrameCount;
	}
	else{
		return;
	}

	//Broodwar->drawLineMap(gameUnit->getPosition(), lastPosition, Colors::Red);

	// Get our starting location
	if (originPosition.x == 0 && originPosition.y == 0) {
		originPosition = gameUnit->getPosition();
	}


	Unitset closeUnits = Broodwar->getUnitsInRadius(lastPosition, 6 * TILE_SIZE, Filter::IsOwned);
	for (auto unit = closeUnits.begin(); unit != closeUnits.end(); unit++){
		if(unit->getType() == UnitTypes::Terran_Marine && unit->getHitPoints() < unit->getInitialHitPoints()){
			gameUnit->attack(unit->getPosition());
			return;
		}
	}

	if(state == CURE_MARINE){
		/*Unitset closeUnits = Broodwar->getUnitsInRadius(lastPosition, 6 * TILE_SIZE, Filter::IsOwned);
		for (auto unit = closeUnits.begin(); unit != closeUnits.end(); unit++){
			if(unit->getType() == UnitTypes::Terran_Marine && unit->getHitPoints() < unit->getInitialHitPoints()){
				gameUnit->attack(unit->getPosition());
				return;
			}
		}*/

		Unitset units = Broodwar->getUnitsInRadius(lastPosition, 2 * TILE_SIZE, Filter::IsOwned);
		Broodwar->drawCircleMap(lastPosition,2 * TILE_SIZE,Color(Colors::White));
		int marines = 0;
		for (auto unit = units.begin(); unit != units.end(); unit++){
			if(unit->getType() == UnitTypes::Terran_Marine){
				marines++;
			}
		}

		Broodwar->drawTextMap(gameUnit->getPosition(),"\nMarines %d", marines);

		if(marines <= 0) {
			updatePositionToCure();
		}
		else{
			gameUnit->attack(lastPosition);
		}
	}
	else{
		updatePositionToCure();
	}

	gameUnit->attack(lastPosition);
	Broodwar->drawLineMap(gameUnit->getPosition(), lastPosition, Colors::Green);
}


void MedicAgent::updatePositionToCure(){
	double closestDist = 100000;
	int closestId = 0;

	Unitset units = Broodwar->self()->getUnits();
	for (Unitset::iterator unit = units.begin(); unit != units.end(); unit++){
		if(unit->getType() == UnitTypes::Terran_Marine && unit->exists() && unit->isCompleted()){
			double dist = unit->getDistance(unit->getPosition());
			if (dist < closestDist){
				closestDist = dist;
				closestId = unit->getID();
			}
		}
	}

	if(closestId != 0){
		state = CURE_MARINE;
		lastPosition = Broodwar->getUnit(closestId)->getPosition();
	}
	else{
		state = NO_TASK;
		lastPosition = originPosition;
	}
}

void MedicAgent::cureInPos(){
	gameUnit->attack(originPosition);
}


bool MedicAgent::isOnAttack(){
	return state == PACKING || state == ATTACKING;
}