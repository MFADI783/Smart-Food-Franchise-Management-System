#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <windows.h>
#include <process.h>
#include "models.h"

using namespace std;

// Helper function for string conversion (replaces to_string for older compilers)
template <typename T>
string toString(T val) {
    stringstream ss;
    ss << val;
    return ss.str();
}

void centerText(string text, int fixedPos = -1, bool newLine = true) {
    int width = 80; 
    int pos = (fixedPos != -1) ? fixedPos : (width - text.length()) / 2;
    if (pos < 0) pos = 0;
    for (int i = 0; i < pos; i++) cout << " ";
    cout << text;
    if (newLine) cout << endl;
}

// Global Synchronization for Console Output
CRITICAL_SECTION console_cs;

void print_safe(string msg) {
    EnterCriticalSection(&console_cs);
    centerText(msg);
    LeaveCriticalSection(&console_cs);
}

class RestaurantSystem {
public:
    LinkedList<Dish> menu;
    ThreadSafeQueue<Order*> orderQueue; // Note: Changed to pointers for easier management
    ThreadSafeQueue<DishTask> kitchenQueue;
    ThreadSafeQueue<Order*> deliveryQueue;
    
    double regularSales = 0;
    double regularWaiterSales = 0;
    long long totalWaitTime = 0;
    int customersServed = 0;
    CRITICAL_SECTION sales_cs;

    HANDLE hManagerRead, hManagerWrite; // Pipe for Customer -> Manager

    RestaurantSystem() {
        InitializeCriticalSection(&console_cs);
        InitializeCriticalSection(&sales_cs);
        loadMenu();
        
        // Creating Pipe
        SECURITY_ATTRIBUTES sa = { sizeof(sa), NULL, TRUE };
        if (!CreatePipe(&hManagerRead, &hManagerWrite, &sa, 0)) {
            cerr << "Pipe Creation Failed!" << endl;
        }
    }

    void loadMenu() {
        ifstream file("menu.txt");
        if (!file.is_open()) {
            cerr << "Could not open menu.txt!" << endl;
            return;
        }
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string name, id_s, price_s, time_s;
            getline(ss, name, ',');
            getline(ss, id_s, ',');
            getline(ss, price_s, ',');
            getline(ss, time_s, ',');
            
            Dish d;
            d.name = name;
            
            stringstream s1(id_s), s2(price_s), s3(time_s);
            s1 >> d.id;
            s2 >> d.price;
            s3 >> d.prepTime;
            
            menu.insert(d);
        }
        file.close();
    }

    void updateSales(double amount, bool isWaiter) {
        EnterCriticalSection(&sales_cs);
        if (isWaiter) regularWaiterSales += amount;
        else regularSales += amount;
        LeaveCriticalSection(&sales_cs);
    }

    void displayMenu(int pad = 20) {
        centerText("----------- OUR MENU -----------", pad);
        centerText("ID\tName\t\tPrice", pad);
        Node<Dish>* temp = menu.getHead();
        while (temp) {
            string tab = (temp->data.name.length() < 8) ? "\t\t" : "\t";
            centerText(toString(temp->data.id) + "\t" + temp->data.name + tab + toString(temp->data.price) + " PKR", pad);
            temp = temp->next;
        }
        centerText("--------------------------------", pad);
    }
};

// Global instance 
RestaurantSystem sys;

// --- Thread Functions ---

void __cdecl customerThread(void* p) {
    Order* ord = (Order*)p;
    
    // Initial dishes were added in main thread
    // No more hardcoded picking here

    ord->startTime = GetTickCount();
    
    // Communication via Pipe
    DWORD written;
    WriteFile(sys.hManagerWrite, &ord, sizeof(Order*), &written, NULL);
    
    print_safe("[CUSTOMER] " + ord->customerName + " (ID: " + ord->customerID + ") placed order #" + toString(ord->orderNum));
    
    // Simulated arrival jitter
    Sleep(rand() % 500);
}

void __cdecl managerThread(void* p) {
    while (true) {
        Order* ord;
        DWORD read;
        if (ReadFile(sys.hManagerRead, &ord, sizeof(Order*), &read, NULL)) {
            print_safe("[MANAGER] Received order #" + toString(ord->orderNum) + ". VIP: " + string(ord->isVIP ? "YES" : "NO"));
            sys.updateSales(ord->totalBill, false);
            
            // Receipt Generation
            string filename = "receipt_" + toString(ord->orderNum) + ".txt";
            ofstream rec(filename.c_str());
            rec << "Customer: " << ord->customerName << " (ID: " << ord->customerID << ")" << endl;
            rec << "Phone: " << ord->phone << endl;
            rec << "Order #" << ord->orderNum << " | VIP: " << (ord->isVIP ? "YES" : "NO") << endl;
            rec << "Total Bill: " << ord->totalBill << " PKR" << endl;
            rec.close();
            
            // Split Order into DishTasks for Multiple Cooks
            Node<Dish>* temp = ord->dishes.getHead();
            int count = 0;
            while(temp) { count++; temp = temp->next; }
            ord->remainingDishes = count;

            temp = ord->dishes.getHead();
            while(temp) {
                DishTask task;
                task.orderNum = ord->orderNum;
                task.dish = temp->data;
                task.parentOrder = ord;
                sys.kitchenQueue.enqueue(task, ord->isVIP); // VIPs go to front
                temp = temp->next;
            }
        }
        Sleep(100);
    }
}

void __cdecl cookThread(void* p) {
    int id = *((int*)p);
    while (true) {
        DishTask task;
        if (sys.kitchenQueue.dequeue(task)) { // deadlock 
            print_safe("[COOK " + toString(id) + "] Preparing " + task.dish.name + " for #" + toString(task.orderNum));
            Sleep(task.dish.prepTime * 1000); // Simulate individual dish prep
            
            EnterCriticalSection(&sys.sales_cs);
            task.parentOrder->remainingDishes--;
            if (task.parentOrder->remainingDishes == 0) {
                print_safe("[SYSTEM] Order #" + toString(task.orderNum) + " is READY for delivery.");
                sys.deliveryQueue.enqueue(task.parentOrder);
            }
            LeaveCriticalSection(&sys.sales_cs);
        }
        Sleep(100);
    }
}

void __cdecl waiterThread(void* p) {
    int id = *((int*)p);
    while (true) {
        Order* ord;
        if (sys.deliveryQueue.dequeue(ord)) {
            print_safe("[WAITER " + toString(id) + "] Delivering order #" + toString(ord->orderNum) + " to " + ord->customerName);
            Sleep(1000); // Simulate delivery
            
            EnterCriticalSection(&sys.sales_cs);
            ord->finishTime = GetTickCount();
            sys.totalWaitTime += (ord->finishTime - ord->startTime);
            sys.customersServed++;
            sys.regularWaiterSales += ord->totalBill;
            LeaveCriticalSection(&sys.sales_cs);
            
            print_safe("[CUSTOMER] " + ord->customerName + " received order #" + toString(ord->orderNum) + ". Satisfied!");
            // delete ord; // Memory cleanup
        }
        Sleep(100);
    }
}

