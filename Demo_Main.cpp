#include "Battle.h"
#include "GUI\GUI.h"
#include "ArrayStack.h"
#include <iostream>
using namespace std;
int main()
{
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
	Battle* pGameBattle = new Battle;
	pGameBattle->RunSimulation();
	delete pGameBattle;
	
	return 0;
}
