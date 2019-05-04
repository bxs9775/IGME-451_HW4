#pragma once
#include <iostream>
#include <mutex>

class Barber
{
public:
	Barber();
	//Barber(int id);
	~Barber();

	void setId(int id);
	int getId();

	//void run();
	//void cut_hair();
private:
	int id;
};

