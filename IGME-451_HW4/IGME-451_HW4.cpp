// IGME-451_HW4.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <mutex>

// Stores the number of chairs, customers, and barbers to use in the process...
int numberOfChairs;
int numberOfCustomers;
int numberOfBarbers;

// Thread management variables
std::mutex customerMutex;
std::mutex barberMutex;
std::mutex chairMutex;
int waiting = 0;
int customersLeft;

void get_haircut() {
	std::cout << "Customer got their hair cut.";
}

void cut_hair() {
	std::cout << "Barber cut a customer's hair.";
}

void Customer(void) {
	chairMutex.lock();
	if (waiting < numberOfChairs) {
		waiting = waiting + 1;
		customerMutex.unlock();
		chairMutex.unlock();
		barberMutex.lock();
		get_haircut();
	}
	else {
		chairMutex.unlock();
	}
}

void Barber(void) {
	while (true) {
		customerMutex.lock();
		chairMutex.lock();
		waiting = waiting - 1;
		barberMutex.unlock();
		chairMutex.unlock();
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
	numberOfCustomers = 5;
	numberOfBarbers = 1;

	std::thread* customers = new std::thread[numberOfCustomers];
	std::thread* barbers = new std::thread[numberOfBarbers];

	for (int i = 0; i < numberOfBarbers; i++) {
		barbers[i] = std::thread(Barber);
	}
	for (int i = 0; i < numberOfCustomers; i++) {
		customers[i] = std::thread(Customer);
	}
	for (int i = 0; i < numberOfCustomers; i++) {
		customers[i].join();
	}

	delete[] barbers;
	delete[] customers;
}
