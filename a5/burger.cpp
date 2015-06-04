#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <cstdlib>
#include <string>
#include <cassert>
#include <algorithm>

const int NUMBER_TABLES = 10; // max number of tables in the restaurant
const int AVG_SEATS_PER_TABLE = 6; // average number of seats per table

struct Order
{
    int table;
    int coffee;
    int coke;
    int burger;
    int salad;
    int id;
};

int orderSerialNumber = 0; // all orders get a unique number (could be the time as well)

// Read sample orders from the specified file and return them as container.
std::vector<Order> takeOrders(char* path)
{
	std::vector<Order> orders;
    
    std::ifstream file(path);
    std::string field, line;
    
    int currentLineNum = 0;
    
    Order order;
    
    while (std::getline(file, line))
    {
        std::istringstream linestream;
        linestream.str(line);
        int fieldNum = 0;
        currentLineNum++;
        
        while (std::getline(linestream, field, ';'))
        {
            try
            {
                switch (fieldNum)
                {
                    case 0:
                        order.table = std::stoi(field);
                        break;
                    case 1:
                        order.coffee = std::stoi(field);
                        break;
                    case 2:
                        order.coke = std::stoi(field);
                        break;
                    case 3:
                        order.burger = std::stoi(field);
                        break;
                    case 4:
                        order.salad = std::stoi(field);
                        break;
                }
            }
            catch (const std::invalid_argument&)
            {
                std::cout << "Couldn't convert entry " << currentLineNum << " correctly (invalid argument)!" << std::endl;
                std::cout << field << std::endl;
            }
            catch (const std::out_of_range&)
            {
                std::cout << "Couldn't convert entry " << currentLineNum << " correctly (out of range)!" << std::endl;
                std::cout << field << std::endl;
            }
            
            fieldNum++;
        }
        order.id = ++orderSerialNumber;
        orders.push_back(order);
    }
    return orders;
}

// To Do:
// Merge the incoming orders into the current main list of orders.
// Keep in mind that the incoming orders are arbitrarily arranged, but
// the main list (currentOrders) has to ensure that its entries have ascending table numbers.
// This way, all orders for the same table can be identified by consecutive orders later.
void processOrders(std::vector<Order>& currentOrders, std::vector<Order>& incomingOrders)
{

}

// To Do:
// Given the list of current orders,
// merge all orders, that is, all orders for the same table are merged into one order.
// Once this is done, the order list contains only one single order per active table.
// Tables for which no orders are given are not represented.
// If two or more orders are merged, their corresponding items (coke, coffee, ...) are just added.
void mergeOrders(std::vector<Order>& currentOrders)
{

}

// To Do:
// The given table wants to pay.
// If it was already paid, nothing happens.
// if it has not paid so far, the price has to be calculated based on
// a simple price list: coffee or coke: 1 Euro, Burger: 5 Euro, Salad: 4 Euro
// Remove the order from the order list and return the price for the given table.
int pay(int table, std::vector<Order>& currentOrders)
{
    return 0;
}

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        std::cout << "not enough arguments - USAGE: burger <SAMPLE DATASET>" << std::endl;
        return -1;	// invalid number of parameters
    }
    
    std::vector<Order> currOrders;
    
    std::cout << "Take new orders.." << std::endl;
	auto newOrders = takeOrders(argv[1]);
    std::cout << "Process orders.." << std::endl;
    processOrders(currOrders, newOrders);
    
    std::cout << "Merge orders.." << std::endl;
    mergeOrders(currOrders);
    
    std::cout << "Payment: " << std::endl;
    const int expectedResults[NUMBER_TABLES] = { 180, 198, 216, 166, 0, 293, 265, 186, 245, 204 };
    int payment = 0;
    for(int tableIdx=0; tableIdx < NUMBER_TABLES; tableIdx++)
    {
        payment = pay(tableIdx, currOrders);
        assert(payment >= 0);
        if(payment != expectedResults[tableIdx])
        {
            std::cout << "WRONG RESULT - Expected: " << expectedResults[tableIdx] << " - Calculated: " << payment << std::endl;
        }
        else
        {
            if(payment == 0)
            {
                std::cout << "No open bill for table " << tableIdx << std::endl;
            }
            else
            {
                std::cout << "Table " << tableIdx << " pays " << payment << " Euro" << std::endl;
            }
        }
    }
    
    return 0;
}
