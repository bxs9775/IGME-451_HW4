// IGME-451_HW4.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>

// Stores the number of chairs, customers, and barbers to use in the process...
int numberOfChairs;
int numberOfCustomers;
int numberOfBarbers;

// Thread management variables
typedef int semaphore;
semaphore customers;
semaphore barbers;
semaphore chairs;
int waiting = 0;

//placeholder method for lock
void down(semaphore mutex) {
}

//placeholder method for lock
void up(semaphore mutex) {
}

void get_haircut() {}

void cut_hair() {}

void Customer(void) {
	down(chairs);
	if (waiting < numberOfChairs) {
		waiting = waiting + 1;
		up(customers);
		up(chairs);
		down(barbers);
		get_haircut();
	}
	else {
		up(chairs);
	}
}

void Barber(void) {
	while (true) {
		down(customers);
		down(chairs);
		waiting = waiting - 1;
		up(barbers);
		up(chairs);
		cut_hair();
	}
}


int main()
{
    /*
	std::cout << "How many chairs -> "; 
	std::cin >> numberOfChairs;
	std::cout << "How many customers -> ";
	std::cin >> numberOfCustomers;
	std::cout << "How many barbers -> ";
	std::cin >> numberOfBarbers;

	if (!numberOfCustomers) {
		std::cout << "There are no customers. End program.\n";
		return;
	}
	if (!numberOfChairs) {
		std::cout << "There are no chairs for customers. End program.\n";
		return;
	}
	if (!numberOfBarbers) {
		std::cout << "There are no barbers on staff. End program.\n";
		return;
	}
	*/

	//Using set values first to get things working...
	numberOfChairs = 5;
	numberOfBarbers = 1;
	numberOfCustomers = 5;

}
