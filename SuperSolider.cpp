#include "SuperSolider.h"
#include <math.h>



SuperSolider::SuperSolider(int Destiny, int health) : Destiny(Destiny), speed(3), health(health)
{
	Distance = 2;
}

int SuperSolider::getDistance()
{
	return Distance;
}

int SuperSolider::getDestiny()
{
	return Destiny;
}

bool SuperSolider::isDead() const
{
	if (health == 0)
		return true;
	else
		return false;
}



void SuperSolider::move()
{
	if (abs(Distance - Destiny) > 3)
	{
		Distance += speed;
	}
	else
		Distance = Destiny;
}

void SuperSolider::receiveDamage(double damage)
{
	if (health - damage <= 0)
		health = 0;
	else
		health -= damage;
}
