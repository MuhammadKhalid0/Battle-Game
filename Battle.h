#pragma once

#include "Enemies\Enemy.h"
#include "Enemies\Freezer.h"
#include "Enemies\Healer.h"
#include "Enemies\Fighter.h"
#include "ArrayStack.h";
#include "Castle\Castle.h"
#include "Generic_DS\Queue.h"
#include "GUI\GUI.h"
#include "PriorityQueue.h"

// it is the controller of the project
class Battle
{
private:
	GUI* pGUI;
	Castle BCastle;
	int EnemyCount=0;	//the actual number of enemies in the game
	int FighterCount=0, FreezerCount=0, HealerCount=0;
	int KilledFighter=0, KilledFreezers=0, KilledHealer=0;
	int FrostedFighter=0, FrostedFreezer=0, FrostedHealer=0;
	int ActiveFighter=0, ActiveHealer=0, ActiveFreezer=0;
	static int CurrentTimeStep;
	//Enemy * BEnemiesForDraw[MaxEnemyCount]; // This Array of Pointers is used for drawing elements in the GUI
								  			// No matter what list type you are using to hold enemies, 
											// you must pass the enemies to the GUI function as an array of enemy pointers. 
											// At every time step, you should update those pointers 
											// to point to the current active enemies 
											// then pass the pointers list to the GUI function

	Queue<Enemy*> Q_Inactive;					//Queue of inactive enemies
	Queue<Enemy*> Q_Killed;						//Queue of killed Enemies
	Queue<Enemy*> Q_Active;						//Queue of active Enemies
	

	/// ==> 
	//	DEMO-related members. Should be removed in phases 1&2
	//Queue for all enemies in the battle
	int DemoListCount;
	Enemy* DemoList[MaxEnemyCount];	//Important: This is just for demo

public:
	
	Battle();
	void AddAllListsToDrawingList(); //Add enemy lists (inactive, active,.....) to drawing list to be displayed on user interface
	Castle * GetCastle();
	void RunSimulation();
	void ActivateEnemies();
	GAME_STATUS InteractiveMode();
	GAME_STATUS StepByStepMode();
	GAME_STATUS SilentMode();

	void AddtoDemoList(Enemy* Ptr); //Add Enemy to the demo queue of enemies (for demo purposes only)
	void Demo_UpdateEnemies();	//Randomly update enemies distance/status (for demo purposes)

	void ImportInputFile();
	void ExportOutputFile(GAME_STATUS gameStatus);

	int getMaxEnemDist(); //gets the distance of maximum number of enemies

	static int getCurrentTimeStep();
	GAME_STATUS runTimeStep();

};