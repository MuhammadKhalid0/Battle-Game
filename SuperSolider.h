#pragma once
class SuperSolider
{
private:
	int Distance;
	const int Destiny;
	const int speed;
	int health;

public:
	SuperSolider(int Destiny, int health);

	//getters
	int getDistance();
	int getDestiny();
	bool isDead() const;

	//setters

	
	//operations
	void receiveDamage(double damage);
	void move();

};