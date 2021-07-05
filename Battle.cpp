#include "Battle.h"
#include <time.h>
#include <fstream>
#include <sstream>
#include "Enemies\Fighter.h"
#include "Enemies\Freezer.h"
#include "Enemies/Healer.h"
#include <string>
#include <stdlib.h>
#include <ctime>
#include <chrono>
#include <thread>

using namespace std;
int Battle::CurrentTimeStep = 0;
Battle::Battle()
{	
	pGUI = NULL;
}

void Battle::AddtoDemoList(Enemy* Ptr)
{
	DemoList[DemoListCount++] = Ptr;

	// Note that this function doesn't allocate any enemy objects
	// It just enqueue a pointer that is already allocated into the queue
}


Castle * Battle::GetCastle()
{
	return &BCastle;
}


void Battle::RunSimulation()
{
	pGUI = new GUI;
	PROG_MODE mode = pGUI->getGUIMode();
	GAME_STATUS status = DRAWN;
	switch (mode)	//Add a function for each mode in next phases
	{
	case MODE_INTR:
		status = InteractiveMode();
		break;
	case MODE_STEP:
		status = StepByStepMode();
		break;
	case MODE_SLNT:
		status = SilentMode();
		break;
	}
	ExportOutputFile(status);
	delete pGUI;
}


//This is just a demo function for project introductory phase
//It should be removed in phases 1&2
GAME_STATUS Battle::InteractiveMode()
{	
	int ActiveCount, FrostedCount;
	pGUI->PrintMessage("Click to start");
	pGUI->waitForClick();
	CurrentTimeStep = 0;
	ImportInputFile();
	AddAllListsToDrawingList();
	pGUI->UpdateInterface(CurrentTimeStep, BCastle.GetHealth(), BCastle.IsFrosted());	//upadte interface to show the initial case where all enemies are still inactive
	GAME_STATUS gameStatus= IN_PROGRESS;
	while (gameStatus == IN_PROGRESS)
	{
		pGUI->waitForClick();
		CurrentTimeStep++;
		gameStatus=runTimeStep();
		pGUI->ResetDrawingList();
		AddAllListsToDrawingList();
		FrostedCount = FrostedFighter + FrostedFreezer + FrostedHealer;
		ActiveCount = Q_Active.getC() - FrostedCount;
		pGUI->UpdateInterface(CurrentTimeStep, BCastle.GetHealth(), BCastle.IsFrosted(), Q_Killed.getC(),
			ActiveCount, FrostedCount, ActiveFighter, ActiveFreezer, ActiveHealer,
			FrostedFighter, FrostedHealer, FrostedFreezer, KilledFighter, KilledFreezers, KilledHealer);
	}
	pGUI->waitForClick();
	return gameStatus;
}

GAME_STATUS Battle::StepByStepMode()
{

	int ActiveCount, FrostedCount;
	CurrentTimeStep = 0;
	ImportInputFile();
	AddAllListsToDrawingList();
	pGUI->UpdateInterface(CurrentTimeStep, BCastle.GetHealth(), BCastle.IsFrosted());	//upadte interface to show the initial case where all enemies are still inactive
	GAME_STATUS gameStatus = IN_PROGRESS;
	while (gameStatus == IN_PROGRESS)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		CurrentTimeStep++;
		gameStatus = runTimeStep();
		pGUI->ResetDrawingList();
		AddAllListsToDrawingList();
		FrostedCount = FrostedFighter + FrostedFreezer + FrostedHealer;
		ActiveCount = Q_Active.getC() - FrostedCount;
		pGUI->UpdateInterface(CurrentTimeStep, BCastle.GetHealth(), BCastle.IsFrosted(), Q_Killed.getC(),
			ActiveCount, FrostedCount, ActiveFighter, ActiveFreezer, ActiveHealer,
			FrostedFighter, FrostedHealer, FrostedFreezer, KilledFighter, KilledFreezers, KilledHealer);
	}
		
	pGUI->waitForClick();
	return gameStatus;
}

GAME_STATUS Battle::SilentMode()
{
	CurrentTimeStep = 0;
	
	ImportInputFile();
	GAME_STATUS gameStatus = IN_PROGRESS;
	while (gameStatus == IN_PROGRESS)
	{
		CurrentTimeStep++;
		gameStatus = runTimeStep();
	}

	pGUI->waitForClick();
	return gameStatus;
}

//Add enemy lists (inactive, active,.....) to drawing list to be displayed on user interface
void Battle::AddAllListsToDrawingList()
{	
	//Add inactive queue to drawing list
	int InactiveCount;
	int ActiveCount;
	int KilledCount;
	int ssCount;
	//Inactive
	Enemy* const* InactiveList = Q_Inactive.toArray(InactiveCount);
	for (int i = 0; i < InactiveCount; i++)
		pGUI->AddToDrawingList(InactiveList[i]);

	//Active Or Frosted
	Enemy* const* ActiveList = Q_Active.toArray(ActiveCount);
	for (int i = 0; i < ActiveCount; i++)
		pGUI->AddToDrawingList(ActiveList[i]);

	//Killed
	Enemy* const* KilledList = Q_Killed.toArray(KilledCount);
	for (int i = 0; i < KilledCount; i++)
		pGUI->AddToDrawingList(KilledList[i]);

	//SS
	SuperSolider* const* SS = BCastle.getPtrSS()->toArray(ssCount);
	for (int i = 0; i < ssCount; i++)
		pGUI->AddToDrawingList(SS[i]);

}

void Battle::ActivateEnemies()
{
	Enemy* pE;
	while (Q_Inactive.peekFront(pE))	//as long as there are more inactive enemies
	{
		if (pE->GetArrvTime() > CurrentTimeStep)	//no more arrivals at current time
			return;

		Q_Inactive.dequeue(pE);	//remove enemy from the queue
		pE->SetStatus(ACTV);	//make status active
		Q_Active.enqueue(pE);
	}
}




void Battle::ImportInputFile()
{
	ifstream fin;
	stringstream stream;
	fin.open("InputFile.txt");
	string alldata, CH, N, CP, M, ID, TYP, AT, H, POW, RLD, SPD;
	

	//Getting first line (Castle)
	getline(fin, alldata);
	stream.str(alldata);
	getline(stream, CH, ' ');
	getline(stream, N,  ' ');
	getline(stream, CP, ' ');
	BCastle.SetHealth(stoi(CH));
	BCastle.SetOriginalHealth(stoi(CH));
	BCastle.SetcasltePower(stoi(CP));
	BCastle.SetmaxAttack(stoi(N));
	BCastle.setFreezingThreshold(BCastle.GetHealth()*0.33);


	//Getting second line (Num of Enemies)
	getline(fin, alldata);
	stream.str(alldata);
	getline(stream, M, ' ');
	EnemyCount = stoi(M);

	//Getting the rest of lines (Enemies)
	while (getline(fin, alldata))
	{
		Enemy* enemy;

		stream.str(alldata);
		getline(stream, ID,  ' ');
		getline(stream, TYP, ' ');
		getline(stream, AT,  ' ');
		getline(stream, H,   ' ');
		getline(stream, POW, ' ');
		getline(stream, RLD, ' ');
		getline(stream, SPD, ' ');

		if (TYP == "0")
		{
			enemy = new Fighter(stoi(ID), stoi(AT), stoi(H), stoi(POW), stoi(SPD), stoi(RLD));
			FighterCount++;
		}
		else if (TYP == "1")
		{
			enemy = new Healer(stoi(ID), stoi(AT), stoi(H), stoi(POW), stoi(SPD), stoi(RLD));
			HealerCount++;
		}
		else
		{
			enemy = new Freezer(stoi(ID), stoi(AT), stoi(H), stoi(POW), stoi(SPD), stoi(RLD));
			FreezerCount++;
		}
		Q_Inactive.enqueue(enemy);
	}
}

void Battle::ExportOutputFile(GAME_STATUS gameStatus)
{
	ofstream fout("outputFile.txt");
	Enemy* ptrEnemey;
	int TotalKilled = Q_Killed.getC();
	int sumFirstShotDelay = 0;
	int sumKillDelay = 0;

	fout << "Game is ";
	if (gameStatus == WIN)
	{
		fout << "WIN\n";
	}
	else if (gameStatus == LOSS)
	{
		fout << "LOSS\n";
	}
	else if (gameStatus == DRAWN)
	{
		fout << "DRAWN\n";
	}
	
	fout << "KTS  ID   FD   KD   LT\n";
	for (int i = 0; i < TotalKilled; i++)
	{

		Q_Killed.dequeue(ptrEnemey);
		
		//outputting KTS
		if (ptrEnemey->getKilledTime() < 10)
		{
			fout << ptrEnemey->getKilledTime() << "    ";
		}
		else if (ptrEnemey->getKilledTime() < 100)
		{
			fout << ptrEnemey->getKilledTime() << "   ";
		}
		else if (ptrEnemey->getKilledTime() < 1000)
		{
			fout << ptrEnemey->getKilledTime() << "  ";
		}

		//outputting ID
		if (ptrEnemey->GetID() < 10)
		{
			fout << ptrEnemey->GetID() << "    ";
		}
		else if (ptrEnemey->GetID() < 100)
		{
			fout << ptrEnemey->GetID() << "   ";
		}
		else if (ptrEnemey->GetID() < 1000)
		{
			fout << ptrEnemey->GetID() << "  ";
		}

		//outputting FD
		sumFirstShotDelay += ptrEnemey->getfirstShotTime() - ptrEnemey->GetArrvTime();
		if (ptrEnemey->getfirstShotTime() - ptrEnemey->GetArrvTime() < 10)
		{
			fout << ptrEnemey->getfirstShotTime() - ptrEnemey->GetArrvTime() << "    ";
		}
		else if (ptrEnemey->getfirstShotTime() - ptrEnemey->GetArrvTime() < 100)
		{
			fout << ptrEnemey->getfirstShotTime() - ptrEnemey->GetArrvTime() << "   ";
		}
		else if (ptrEnemey->getfirstShotTime() - ptrEnemey->GetArrvTime() < 1000)
		{
			fout << ptrEnemey->getfirstShotTime() - ptrEnemey->GetArrvTime() << "  ";
		}

		//outputting KD
		sumKillDelay += ptrEnemey->getKilledTime() - ptrEnemey->getfirstShotTime();
		if (ptrEnemey->getKilledTime() - ptrEnemey->getfirstShotTime() < 10)
		{
			fout << ptrEnemey->getKilledTime() - ptrEnemey->getfirstShotTime() << "    ";
		}
		else if (ptrEnemey->getKilledTime() - ptrEnemey->getfirstShotTime() < 100)
		{
			fout << ptrEnemey->getKilledTime() - ptrEnemey->getfirstShotTime() << "   ";
		}
		else if (ptrEnemey->getKilledTime() - ptrEnemey->getfirstShotTime() < 1000)
		{
			fout << ptrEnemey->getKilledTime() - ptrEnemey->getfirstShotTime() << "  ";
		}
		
		//outputting LT
		if (ptrEnemey->getKilledTime() - ptrEnemey->GetArrvTime() < 10)
		{
			fout << ptrEnemey->getKilledTime() - ptrEnemey->GetArrvTime() << "    ";
		}
		else if (ptrEnemey->getKilledTime() - ptrEnemey->GetArrvTime() < 100)
		{
			fout << ptrEnemey->getKilledTime() - ptrEnemey->GetArrvTime() << "   ";
		}
		else if (ptrEnemey->getKilledTime() - ptrEnemey->GetArrvTime() < 1000)
		{
			fout << ptrEnemey->getKilledTime() - ptrEnemey->GetArrvTime() << "  ";
		}
		fout << endl;
	}

	fout << BCastle.getTotalDamage() << endl;

	if (gameStatus == WIN)
	{
		fout << "Total Enemies = " << TotalKilled << endl;
		fout << "Average First-Shot Delay = " << 1.0*sumFirstShotDelay/TotalKilled << endl;
		fout << "Average Kill Delay = " << 1.0 * sumKillDelay / TotalKilled << endl;
	}
	else
	{
		fout << "Killed Enemies = " << TotalKilled << endl;
		fout << "Alive Enemies = " << EnemyCount - TotalKilled << endl;
		fout << "Average First-Shot Delay for killed = " << 1.0 * sumFirstShotDelay / TotalKilled << endl;
		fout << "Average Kill Delay for killed = " << 1.0 * sumKillDelay / TotalKilled << endl;
	}
}

int Battle::getMaxEnemDist()
{
	Enemy* enemy;
	int Active = Q_Active.getC();
	Queue<Dist*> Q_distance;
	for (int i = 0; i < 60; i++)
	{
		Dist* distance = new Dist;
		distance->position = i + 1;
		Q_distance.enqueue(distance);
	}
	Dist* distance;
	for (int i = 0; i < Active; i++)
	{
		Q_Active.dequeue(enemy);
		int position = enemy->GetDistance();
		for (int i = 0; i < 60; i++)
		{
			Q_distance.dequeue(distance);
			if (distance->position == position)
			{
				distance->number++;
				Q_distance.enqueue(distance);
				break;
			}
			Q_distance.enqueue(distance);
		}
		Q_Active.enqueue(enemy);
	}
	int max = 0;
	int index = 2;
	for (int i = 0; i < 60; i++)
	{
		Q_distance.dequeue(distance);
		if (max < distance->number)
		{
			max = distance->number;
			index = distance->position;
		}
	}
	return index;
}

int Battle::getCurrentTimeStep()
{
	return CurrentTimeStep;
}




GAME_STATUS Battle::runTimeStep()
{
	//enemies activate, enemies walking, enemies acting, enemies reloading, enemy picking criteria
	ActivateEnemies();
	Queue<Healer*> Q_ActiveHealer;
	Queue<Freezer*> Q_ActiveFreezer;
	PriorityQueue<Fighter*> Q_ActiveFighter;
	ArrayStack<Healer*> S_ActiveHealer;
	Queue<Enemy*> ff;
	Fighter* fighter;
	Freezer* freezer;
	Healer* healer;
	Enemy* active;
	int Active = Q_Active.getC();
	FrostedFighter = 0;
	FrostedHealer = 0;
	FrostedFreezer = 0;
	int thresholdTimeSteps = 0; //used to count steps after sending SS

	if (!(BCastle.getPtrSS()->isEmpty()))
	{
		thresholdTimeSteps++;
	}
	else
	{
		thresholdTimeSteps = 0;
	}

	for (int i = 0; i < Active; i++)
	{
		Q_Active.dequeue(active);
		if (!(active->isFrosted()))
		{
			active->Move();
			active->meltIce();
		}
		fighter = dynamic_cast<Fighter*> (active);
		freezer = dynamic_cast<Freezer*> (active);
		healer = dynamic_cast<Healer*> (active);
		if (fighter)
		{
			ff.enqueue(fighter);
			Q_ActiveFighter.insert(fighter, fighter->getPriority());
			if (fighter->isFrosted())
			{
				if (!(fighter->reduceFrostedTime()))
					FrostedFighter++;
			}
			else if (fighter->getReloading() == 0)
			{
				fighter->attackCastle(&BCastle);
				fighter->setReloading();
			}
			else fighter->decrementReload();
		}
		else if (freezer)
		{
			ff.enqueue(freezer);
			Q_ActiveFreezer.enqueue(freezer);
			if (freezer->isFrosted())
			{
				if (!(freezer->reduceFrostedTime()))
					FrostedFreezer++;
			}
			else if (freezer->getReloading() == 0)
			{
				freezer->frostCastle(&BCastle);
				freezer->setReloading();
			}
			else freezer->decrementReload();
		}
		else if (healer)
		{
			S_ActiveHealer.push(healer);
		}
		Q_Active.enqueue(active);
	}
	ActiveHealer = S_ActiveHealer.getCount();
	ActiveFighter = Q_ActiveFighter.size();
	ActiveFreezer = Q_ActiveFreezer.getC();
	for (int i = 0; i < ActiveHealer; i++)
	{
		S_ActiveHealer.pop(healer);
		if (healer->isFrosted())
		{
			if (!(healer->reduceFrostedTime()))
				FrostedHealer++;
		}
		else
		{
			int ffnum = ff.getC();
			for (int i = 0; i < ffnum; i++)
			{
				ff.dequeue(active);
				if (active->GetDistance() == healer->GetDistance()
					|| abs(active->GetDistance() - healer->GetDistance()) == 1
					|| abs(active->GetDistance() - healer->GetDistance()) == 2)
					healer->healEnemy(active);
				ff.enqueue(active);
			}
		}
		Q_ActiveHealer.enqueue(healer);
	}


	//castle attacking
	int n = BCastle.GetmaxAttack();
	std:: srand(time(0));
	int randomnum = rand();
	int newkilledfighter=0;
	int newkilledhealer = 0;
	int newkilledfreezer = 0;
	if ((randomnum % 2) == 0)
	{
		int firenum = n * 0.8;
		for (int i = 0; i < firenum; i++)
		{
			if (Q_ActiveFighter.size() == 0)
				break;
			Q_ActiveFighter.dequeueMax(fighter);
			ENMY_STATUS preType = (fighter->GetStatus());
			if (BCastle.attackEnemy(fighter))
			{
				Q_Killed.enqueue(fighter);
				KilledFighter++;
				newkilledfighter++;
				if (preType == FRST)
					FrostedFighter--;
			}
		}
		if (ActiveFighter < firenum)
		{
			for (int i = 0; i < firenum - ActiveFighter; i++)
			{
				if (Q_ActiveHealer.getC() == 0)
					break;
				Q_ActiveHealer.dequeue(healer);
				ENMY_STATUS preType = (healer->GetStatus());
				if (BCastle.attackEnemy(healer))
				{
					Q_Killed.enqueue(healer);
					BCastle.useHealerTools(healer);
					KilledHealer++;
					newkilledhealer++;
					if (preType == FRST)
						FrostedHealer--;
				}
			}

		}
		if ((ActiveFighter + ActiveHealer) < firenum)
		{
			for (int i = 0; i < (firenum - (ActiveFighter + ActiveHealer)); i++)
			{
				if (Q_ActiveFreezer.getC() == 0 || Q_ActiveFreezer.getC() < i)
					break;
				Q_ActiveFreezer.dequeue(freezer);
				ENMY_STATUS preType = (freezer->GetStatus());
				if (BCastle.attackEnemy(freezer))
				{
					Q_Killed.enqueue(freezer);
					KilledFreezers++;
					newkilledfreezer++;
					if (preType == FRST)
						FrostedFreezer--;
				}
			}
		}
		int icenum = n - firenum;
		int iced = 0;
		if (ActiveFighter > firenum)
		{
			for (int i = 0; i < icenum; i++)
			{
				if (Q_ActiveFighter.size() == 0)
					break;
				Q_ActiveFighter.dequeueMax(fighter);
				if (!fighter->isFrosted())
					if (BCastle.frostEnemy(fighter))
					{
						FrostedFighter++;
						iced++;
					}
			}
		}
		if (ActiveFighter < n)
		{
			icenum = icenum - iced;
			iced = 0;
			for (int i = 0; i < icenum; i++)
			{
				if (Q_ActiveHealer.getC() == 0)
					break;
				Q_ActiveHealer.dequeue(healer);
				if (!healer->isFrosted())
					if (BCastle.frostEnemy(healer))
					{
						FrostedHealer++;
						iced++;
					}
			}
		}
		icenum = icenum - iced + firenum - ActiveFighter - ActiveHealer;
		for (int i = firenum - ActiveFighter - ActiveHealer; i < ActiveFreezer; i++) //HERE
		{
			if (Q_ActiveFreezer.isEmpty())
				break;
			Q_ActiveFreezer.dequeue(freezer);
			if (i < icenum)
				if (!freezer->isFrosted())
					if (BCastle.frostEnemy(freezer))
						FrostedFreezer++;
		}
	}
	else
	{
		for (int i = 0; i < n; i++)
		{
			if (Q_ActiveFighter.size() == 0)
				break;
			Q_ActiveFighter.dequeueMax(fighter);
			ENMY_STATUS preType = (fighter->GetStatus());
			if (BCastle.attackEnemy(fighter))
			{
				Q_Killed.enqueue(fighter);
				KilledFighter++;
				newkilledfighter++;
				if (preType == FRST)
					FrostedFighter--;
			}
		}
		if (ActiveFighter < n)
		{
			for (int i = 0; i < n - ActiveFighter; i++)
			{
				if (Q_ActiveHealer.getC() == 0)
					break;
				Q_ActiveHealer.dequeue(healer);
				ENMY_STATUS preType = (healer->GetStatus());
				if (BCastle.attackEnemy(healer))
				{
					Q_Killed.enqueue(healer);
					KilledHealer++;
					newkilledhealer++;
					if (preType == FRST)
						FrostedHealer--;
				}
			}

		}
		if ((ActiveFighter + ActiveHealer) < n)
		{
			for (int i = 0; i < ActiveFreezer; i++)
			{
				if (Q_ActiveFreezer.getC() == 0)
					break;
				Q_ActiveFreezer.dequeue(freezer);
				if (i < n - (ActiveFighter + ActiveHealer))
				{
					ENMY_STATUS preType = (freezer->GetStatus());
					if (BCastle.attackEnemy(freezer))
					{
						Q_Killed.enqueue(freezer);
						KilledFreezers++;
						newkilledfreezer++;
						if (preType == FRST)
							FrostedFreezer--;
					}
				}
			}
		}
	}
	ActiveFighter = ActiveFighter- newkilledfighter-FrostedFighter;
	ActiveFreezer =ActiveFreezer- newkilledfreezer-FrostedFreezer;
	ActiveHealer = ActiveHealer - newkilledhealer - FrostedHealer;
	for (int i = 0; i < Active; i++)
	{
		Q_Active.dequeue(active);
		if (!(active->isDead()))
			Q_Active.enqueue(active);
	}

	//add the logic of SS here
	if (BCastle.checkThreshold() && thresholdTimeSteps % 5 == 0)
	{
		BCastle.sendSS(getMaxEnemDist());
	}


	if (BCastle.GetHealth() == 0 && EnemyCount == Q_Killed.getC())
		return DRAWN;
	//Check if the castle destroyed return loss
	if (BCastle.GetHealth() == 0)
		return LOSS;

	//check if all enemies killed return win
	if (EnemyCount == Q_Killed.getC())
		return WIN;
	else return IN_PROGRESS;
}

