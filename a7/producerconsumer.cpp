
#include <iostream>
#include <chrono>
#include <thread>
#include <queue>
#include <list>
#include <random>
#include <mutex>
#include <algorithm>
#include <string>


using namespace std::chrono;

// uint64_t is defined in stdint.h -> typedef unsigned long long uint64_t
// -> it's a standardized abbreviation/shortcut for unsigned long long for convinience

auto mainThreadId = std::this_thread::get_id();
const int threshold = 4000;

// print a text to console (thread safe)
void printToConsole(const std::string &string)
{
	static std::mutex coutMutex;

	//do this calculation outside of locked mutex, so locked time is shorter
	std::string indentation = "";
	if (std::this_thread::get_id() != mainThreadId)
	{
		indentation = "\t\t\t";
	}

	coutMutex.lock();
	std::cout << indentation << string << std::endl;
	coutMutex.unlock();
}

//Implement a thread safe queue, because we don't know if underlying class is thread safe
template<typename T, class Container = std::deque<T>>
class ThreadSafeQueue
{
public:
	bool empty() const
	{
		queueMutex_.lock();
		bool isEmpty = queue_.empty();
		queueMutex_.unlock();

		return isEmpty;
	}

	//normally you would call front() and then pop() on normal queue, but because of possible changes between both calls, we combine them
	T pop_front()
	{
		queueMutex_.lock();
		auto item = queue_.front();
		queue_.pop();
		queueMutex_.unlock();

		return item;
	}

	typename Container::size_type size() const
	{
		queueMutex_.lock();
		auto size = queue_.size();
		queueMutex_.unlock();

		return size;
	}

	void push(const T& value)
	{
		queueMutex_.lock();
		queue_.push(value);
		queueMutex_.unlock();
	}

protected:
	std::queue<T, Container> queue_;
	mutable std::mutex queueMutex_; //Mutex to prevent data races on queue
};

// Prototypical Item class - could contain any application specific data.
class Item
{
public:
	Item()
		: uid_{counter_++}
		, spawntime_{system_clock::now()}
	{
		++items_;
		printToConsole("Item " + std::to_string(uid_) + " created");
	}

	~Item()
	{
		--items_;
		printToConsole("Item " + std::to_string(uid_) + " deleted");
	}

	// returns the item's unique id
	uint64_t uid() const
	{
		return uid_;
	}

	// returns the item's lifetime until now
	uint64_t lifeTime() const
	{
		return duration_cast<milliseconds>(system_clock::now() - spawntime_).count();
	}

	// returns the total number of Item objects constructed so far
	static uint64_t items()
	{
		return items_;
	}

protected:
	uint64_t uid_; // item id
	system_clock::time_point spawntime_; // item creation time
	static uint64_t items_; // items_ is used to count the number of instances of Item
	static uint64_t counter_; // counter for incrementing unique item ids
};

uint64_t Item::items_ = 0;
uint64_t Item::counter_ = 1;

class Consumer
{
public:
	Consumer(ThreadSafeQueue<std::shared_ptr<Item>> &queue) : queue_{queue}, terminate_{false} { }

	~Consumer()
	{
	}

	void work()
	{
		while (!terminate_)
		{
			// make worker thread sleep for short random time
			std::this_thread::sleep_for(milliseconds(rand() % 200 + 1));

			cleanup();
			consume();
		}
		printToConsole("TERMINATED");
	}

	void consume()
	{
		int maxItemsProcessed = 10;
		while (!queue_.empty() && todo_.size() < maxItemsProcessed) {
			auto item = queue_.pop_front();
			todo_.push_back(item);
		}
	}

	void cleanup()
	{
		printToConsole("CLEANUP");

		// remove and delete all items with their actual lifetime exceeding a threshold.
		todo_.erase(std::remove_if(todo_.begin(), todo_.end(), [] (std::shared_ptr<Item> item) {
			return item->lifeTime() > threshold;
		}), todo_.end());
		// Note: unless you remove some items, no new items will be processed obviously ...
	}

	void terminate()
	{
		printToConsole("TERMINATE CONSUMER");
		terminate_ = true;
	}

protected:
	ThreadSafeQueue<std::shared_ptr<Item>> &queue_; // reference to the producer and the source of items
	std::list<std::shared_ptr<Item>> todo_; // own list of items currently being processed
	bool terminate_; // flag that controls the termination of the consumer thread
};


int main(int /*argc*/, char * /*argv*/[])
{
	// Note: Raw pointers shouldn't be stored in std containers
	ThreadSafeQueue<std::shared_ptr<Item>> queue; // queue of items that are to be processed/consumed by worker thread

	// instantiate a single Consumer item and run its work method in a local thread, the consumer thread
	auto consumer = new Consumer{queue};
	std::thread t_work(&Consumer::work, consumer);

	// reset random generator, using the system time
	srand(static_cast<unsigned int>(time(0)));

	// in the loop, the producer produces item objects
	auto prob = 500;
	while (rand() % --prob) // prob is used as a hack to randomly and deterministically terminate 
	{
		// make producer thread sleep for short random time
		std::this_thread::sleep_for(milliseconds(rand() % 100 + 1));

		// generate a small number of items that are to be processed by the consumer
		int numItems = rand() % 5 + 1;
		for (int i = 0; i < numItems; ++i) {
			std::shared_ptr<Item> item(new Item());
			queue.push(item);
		}
	}
	printToConsole("TERMINATED");

	// delete remaining items, i.e., items that are still in the queue
	// and are not processed by the consumer; they got lost.
	while (!queue.empty()) {
		std::shared_ptr<Item> item = queue.pop_front();
		item.reset();
	}

	// consumer threads has to be stopped as well
	consumer->terminate(); // (soft)terminate the consumer thread -> see while loop in Cosumer::work
	t_work.join(); // block until consumer thread has terminated

	// make sure to also delete items left in consumer
	delete consumer;

	// number of items left should be 0 -> precisely counted by Items constructor and destructor
	std::cout << std::endl << Item::items() << " items left in memory ..." << std::endl;

	return 0;
}