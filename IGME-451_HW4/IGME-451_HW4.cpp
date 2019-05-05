// IGME-451_HW4.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <mutex>

// Macro for the process of checking if their are customers left and releasing mutexes
#define CHECK_CUSTOMERS_LEFT \
if (customersLeft <= 0) {\
	std::call_once(barbersEnded,[](){customerReady.notify_all();});\
	l.unlock();\
	chairMutex.unlock();\
	return;\
}

// Stores the number of chairs, customers, and barbers to use in the process...
int numberOfChairs;
int numberOfCustomers;
int numberOfBarbers;

// *** Mutex variables ***
// Mutex for controlling a barber's access to information on the customers
std::mutex cMutex;
// Mutex for controlling a customer's access to information on the barbers
std::mutex bMutex;
// Mutex protecting waiting variable which keeps track of chair occupency
std::mutex chairMutex;
// Mutex protecting print operations to std::cout.
std::mutex printMutex;

// Variables for keeping track where different threads are.
int waiting = 0;
int customersLeft;
int barbersWaiting = 0;

// Other mutex/thread related variables
std::condition_variable customerReady;
std::condition_variable barberReady;
std::once_flag barbersEnded;



class Customer {
public:
	Customer() :Customer(-1) {

	}

	Customer(int value) {
		id = value;
	}

	~Customer() {

	}

	// Prints out the fact that the customer got a hair cut.
	void get_haircut() {
		printMutex.lock();
		std::cout << "Customer #" << id << " got their hair cut." << std::endl;
		printMutex.unlock();
	}

	// Function for running the customer process.
	void run() {
		chairMutex.lock();
		if (waiting < numberOfChairs) {
			// if there are chairs sit down and wait for barber
			std::unique_lock<std::mutex> l(bMutex);

			// Lets the barbers know its ready
			waiting = waiting + 1;
			customerReady.notify_one();

			chairMutex.unlock();

			//Waits for a barber to be ready
			barberReady.wait(l, [this]() { return barbersWaiting > 0; });
			--barbersWaiting;

			get_haircut();

			l.unlock();
		}
		else {
			// if the chairs are full leave
			--customersLeft;

			printMutex.lock();
			std::cout << "Customer #" << id << " couldn't find an empty chair and left." << std::endl;
			printMutex.unlock();

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
		std::cout << "B #" << value << " - created." << std::endl;
		id = value;
	}

	// Prints out the fact the barber is cutting hair
	void cut_hair() {
		printMutex.lock();
		std::cout << "Barber #" << id << " snips away with their shears." << std::endl;
		printMutex.unlock();
	}
	
	// Runs the barber's loop that checks if customers are ready and cuts their hair.
	void run(){
		while (true) {
			//Reduces but does not elimate the risk that the barber continues on with a new customer 
			//before the current customer registers that their hair is cut
			bMutex.lock();
			bMutex.unlock();


			std::unique_lock<std::mutex> l(cMutex);
			// Waits until a at least one customer is ready or all the customers have left...
			customerReady.wait(l, [this]() { return waiting > 0 | customersLeft <= 0; });
			chairMutex.lock();
			CHECK_CUSTOMERS_LEFT
			// Lets customer know its ready
			++barbersWaiting;
			barberReady.notify_one();

			//Updates customer info
			waiting = waiting - 1;
			--customersLeft;
			chairMutex.unlock();

			cut_hair();

			chairMutex.lock();
			CHECK_CUSTOMERS_LEFT
			chairMutex.unlock();

			l.unlock();
		}
	}
private:
	int id;
};

int main()
{
	barbersWaiting = 0;

	// Gets the number of chairs, customers, and barbers from user.
	std::cout << "How many chairs -> "; 
	std::cin >> numberOfChairs;
	std::cout << "How many customers -> ";
	std::cin >> numberOfCustomers;
	std::cout << "How many barbers -> ";
	std::cin >> numberOfBarbers;

	// Checks if values are > 0
	if (!numberOfCustomers) {
		std::cout << "There are no customers. End program.\n";
		return -1;
	}
	if (!numberOfChairs) {
		std::cout << "There are no chairs for customers. End program.\n";
		return -1;
	}
	if (!numberOfBarbers) {
		std::cout << "There are no barbers on staff. End program.\n";
		return -1;
	}

	customersLeft = numberOfCustomers;

	// Sets up cThreads (customer threads) and bThreads (barber threads)
	std::thread* cThreads = new std::thread[numberOfCustomers];
	std::thread* bThreads = new std::thread[numberOfBarbers];

	for (int i = 0; i < numberOfBarbers; i++) {
		bThreads[i] = std::thread(&Barber::run,Barber(i));
	}
	for (int i = 0; i < numberOfCustomers; i++) {
		cThreads[i] = std::thread(&Customer::run,Customer(i));
	}

	// calls join on threads to sycronise the program at the end...
	for (int i = 0; i < numberOfCustomers; i++) {
		cThreads[i].join();
	}
	for (int i = 0; i < numberOfBarbers; i++) {
		bThreads[i].join();
	}

	// Cleans up arrays.
	delete[] cThreads;
	delete[] bThreads;
}
