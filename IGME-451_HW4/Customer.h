#pragma once
#include <iostream>
#include <mutex>

class Customer
{
public:
	Customer();
	//Customer(int id);
	~Customer();

	void setId(int id);
	int getId();

	//void run();
	//void get_haircut();

private:
	int id;
};

