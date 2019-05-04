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
std::mutex cMutex;
std::mutex bMutex;
std::mutex chairMutex;
int waiting = 0;
int customersLeft;

int barbersWaiting = 0;
std::condition_variable customerReady;
std::condition_variable barberReady;

class Customer {
public:
	Customer() :Customer(-1) {

	}

	Customer(int value) {
		std::cout << "C #" << id << " - created." << std::endl;
		id = value;
	}

	~Customer() {

	}

	void get_haircut() {
		std::cout << "Customer #" << id << " got their hair cut." << std::endl;
	}

	void run() {
		std::cout << "C #" << id << " - started." << std::endl;
		chairMutex.lock();
		if (waiting < numberOfChairs) {
			std::unique_lock<std::mutex> l(bMutex);
			waiting = waiting + 1;
			//std::cout << "C #" << id << " - customers waiting = " << waiting << std::endl;
			customerReady.notify_one();
			chairMutex.unlock();
			barberReady.wait(l, [this]() { return barbersWaiting > 0; });
			get_haircut();
			//std::cout << "C #" << id << " - waiting = " << waiting << std::endl;
			l.unlock();
			std::cout << "C #" << id << " - stopped." << std::endl;
		}
		else {
			--customersLeft;
			std::cout << " #" << this->id << " - stopped." << std::endl;
			chairMutex.unlock();
		}
	}
private:
	int id;
};

class Barber {
public:
	Barber() :Barber(-1) {

	}

	Barber(int value) {
		std::cout << "B #" << id << " - created." << std::endl;
		id = value;
	}

	void cut_hair() {
		std::cout << "Barber #" << id << " snips away with their shears." << std::endl;
	}
	
	void run(){
		while (true) {
			std::cout << "B #" << id << " - started." << std::endl;
			std::unique_lock<std::mutex> l(cMutex);
			++barbersWaiting;
			barberReady.notify_one();
			//std::cout << "B #" << this->id << " - waiting = " << waiting << std::endl;
			customerReady.wait(l, [this]() { return waiting > 0; });
			//std::cout << "B #" << this->id << " - customers waiting = " << waiting << std::endl;
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
private:
	int id;
};

int main()
{
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

	std::thread* cThreads = new std::thread[numberOfCustomers];
	std::thread* bThreads = new std::thread[numberOfBarbers];

	for (int i = 0; i < numberOfBarbers; i++) {
		bThreads[i] = std::thread(&Barber::run,Barber(i));
	}
	for (int i = 0; i < numberOfCustomers; i++) {
		cThreads[i] = std::thread(&Customer::run,Customer(i));
	}

	for (int i = 0; i < numberOfCustomers; i++) {
		cThreads[i].join();
	}
	for (int i = 0; i < numberOfBarbers; i++) {
		bThreads[i].join();
	}

	delete[] cThreads;
	delete[] bThreads;
}
