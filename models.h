#ifndef MODELS_H
#define MODELS_H

#include <string>
#include "CustomStructures.h"

struct Dish {
    int id;
    std::string name;
    double price;
    int prepTime; // in seconds
};

struct Order {
    int orderNum;
    std::string customerName;
    std::string customerID;
    std::string phone;
    LinkedList<Dish> dishes;
    double totalBill;
    bool isVIP;
    DWORD startTime; // for metrics
    DWORD finishTime;
    int remainingDishes; // for order splitting
};

struct DishTask {
    int orderNum;
    Dish dish;
    Order* parentOrder;
};

struct SalesRecord {
    int orderNum;
    double amount;
    std::string waiterName;
};

#endif
