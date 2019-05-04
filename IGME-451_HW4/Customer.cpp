#include "pch.h"
#include "Customer.h"


Customer::Customer()
{
	this->id = id;
}
/*
Customer::Customer():Customer(-1)
{
}

Customer::Customer(int id)
{
	this->id = id;
}
*/

Customer::~Customer()
{
}

void Customer::setId(int id)
{
	this->id = id;
}

int Customer::getId()
{
	return id;
}

/*
void Customer::run()
{
	// Hmm...
}

void Customer::get_haircut() {
	std::cout << "Customer #" << id << " got their hair cut." << std::endl;
}
*/