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
std::mutex lock;
//std::mutex customerMutex;
//std::mutex barberMutex;
std::mutex chairMutex;
int waiting = 0;
int customersLeft;

int customersWaiting = 0;
int barbersWaiting = 0;
std::condition_variable customerReady;
std::condition_variable barberReady;

struct Customer {
	int id;

	Customer(int value) {
		id = value;
	}

	void get_haircut() {
		
	}

	void run() {
		std::cout << "C #" << this->id << " - started." << std::endl;
		chairMutex.lock();
		if (waiting < numberOfChairs) {
			std::unique_lock<std::mutex> l(lock);
			waiting = waiting + 1;
			++customersWaiting;
			std::cout << "C #" << this->id << " - customers waiting = " << customersWaiting << std::endl;
			customerReady.notify_one();
			chairMutex.unlock();
			barberReady.wait(l, [this]() { return barbersWaiting > 0; });
			get_haircut();
			--customersWaiting;
			std::cout << "C #" << this->id << " - customers waiting = " << customersWaiting << std::endl;
			l.unlock();
			std::cout << "C #" << this->id << " - stopped." << std::endl;
		}
		else {
			--customersLeft;
			std::cout << " #" << this->id << " - stopped." << std::endl;
			chairMutex.unlock();
		}
	}
};

struct Barber {
	int id;

	Barber(int value) {
		id = value;
	}

	void cut_hair() {
		std::cout << "Barber #" << this->id << " snips away with their shears." << std::endl;
	}
	
	void run(){
		while (true) {
			std::cout << "B #" << this->id << " - started." << std::endl;
			std::unique_lock<std::mutex> l(lock);
			++barbersWaiting;
			barberReady.notify_one();
			std::cout << "B #" << this->id << " - customers waiting = " << customersWaiting << std::endl;
			customerReady.wait(l, [this]() { return customersWaiting > 0; });
			std::cout << "B #" << this->id << " - customers waiting = " << customersWaiting << std::endl;
			chairMutex.lock();
			waiting = waiting - 1;
			--customersLeft;
			chairMutex.unlock();
			cut_hair();
			--barbersWaiting;
			l.unlock();
			chairMutex.lock();
			if (customersLeft <= 0) {
				chairMutex.unlock();
				std::cout << "B #" << this->id << " - stopped." << std::endl;
				return;
			}
			chairMutex.unlock();
		}
	}
};

int main()
{
	customersWaiting = 0;
	barbersWaiting = 0;

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

	customersLeft = numberOfCustomers;

	std::thread* customers = new std::thread[numberOfCustomers];
	std::thread* barbers = new std::thread[numberOfBarbers];

	for (int i = 0; i < numberOfBarbers; i++) {
		barbers[i] = std::thread(&Barber::run,&Barber(i));
	}
	for (int i = 0; i < numberOfCustomers; i++) {
		customers[i] = std::thread(&Customer::run,&Customer(i));
	}

	for (int i = 0; i < numberOfCustomers; i++) {
		customers[i].join();
	}
	for (int i = 0; i < numberOfBarbers; i++) {
		barbers[i].join();
	}

	delete[] customers;
	delete[] barbers;
}
