#include <iostream>
#include <windows.h>
#include <ctime>
#include "system_logic.cpp"

void printHeader() {
    system("cls");
    system("color 0B"); 
    for(int i=0; i<8; i++) cout << endl; // Increased vertical padding
    centerText("                                                ==========================================================");
    centerText("                                                |      SMART FOOD FRANCHISE MANAGEMENT SYSTEM (LTD)      | ");
    centerText("                                                ==========================================================");
    centerText("                                                |                     System Online                      |");
    centerText("                                                ----------------------------------------------------------");
}

int main() {
    srand(time(NULL));
    printHeader();

    // Start Manager Thread
    _beginthread(managerThread, 0, NULL);
    
    // Start Cook Threads
    static int cookId1 = 1, cookId2 = 2;
    _beginthread(cookThread, 0, &cookId1);
    _beginthread(cookThread, 0, &cookId2);

    // Start Waiter Threads
    static int waiterId1 = 1, waiterId2 = 2;
    _beginthread(waiterThread, 0, &waiterId1);
    _beginthread(waiterThread, 0, &waiterId2);

    // UI Menu
    int choice;
    int orderCounter = 101;

    int menuPadding = 48.5;
    while (true) {
        centerText("1. Place New Customer Order (Simulate)                   |", menuPadding);
        centerText("2. View End-of-Day Sales Report                          |", menuPadding);
        centerText("3. Exit                                                  |", menuPadding);
        centerText("4. Run Peak-Hour Simulation (Stress Test)                |", menuPadding);
		centerText("----------------------------------------------------------", menuPadding);
        cout << endl;
        centerText("Choice: ", menuPadding, false);
        cin >> choice;

        if (choice == 1) {
            printHeader();
            sys.displayMenu(menuPadding);
            Order* newOrder = new Order();
            newOrder->orderNum = orderCounter++;
            
            centerText("--- CUSTOMER INFORMATION ---", menuPadding);
            centerText("Name: ", menuPadding, false); cin >> newOrder->customerName;
            centerText("ID: ", menuPadding, false); cin >> newOrder->customerID;
            centerText("Phone: ", menuPadding, false); cin >> newOrder->phone;
            centerText("Is VIP? (1 for Yes, 0 for No): ", menuPadding, false); cin >> newOrder->isVIP;
            
            int dishId;
            cout << endl;
            centerText("--- SELECT DISHES (0 to finish) ---", menuPadding);
            while (true) {
                centerText("Dish ID: ", menuPadding, false);
                cin >> dishId;
                if (dishId == 0) break;
                
                Node<Dish>* temp = sys.menu.getHead();
                bool found = false;
                while (temp) {
                    if (temp->data.id == dishId) {
                        newOrder->dishes.insert(temp->data);
                        newOrder->totalBill += temp->data.price;
                        centerText(" + Added: " + temp->data.name, menuPadding);
                        found = true;
                        break;
                    }
                    temp = temp->next;
                }
                if (!found) centerText(" [!] Invalid Dish ID!", menuPadding);
            }
            
            if (newOrder->totalBill > 0) {
                _beginthread(customerThread, 0, (void*)newOrder);
                centerText("\n[SUCCESS] Order #" + toString(newOrder->orderNum) + " sent to Manager.", menuPadding);
            } else {
                centerText("\n[CANCELLED] No items selected.", menuPadding);
                delete newOrder;
            }
            cout << "------------------------------------------" << endl;
            system("pause");
            printHeader();
        } 
        else if (choice == 2) {
            printHeader();
            centerText("--- SYSTEM PERFORMANCE REPORT ---", menuPadding);
            centerText("Total Revenue (Manager): " + toString(sys.regularSales) + " PKR", menuPadding);
            centerText("Total Collected (Waiters): " + toString(sys.regularWaiterSales) + " PKR", menuPadding);
            centerText("Customers Served: " + toString(sys.customersServed), menuPadding);
            
            double avgWait = (sys.customersServed > 0) ? (double)sys.totalWaitTime / sys.customersServed / 1000.0 : 0;
            centerText("Average Wait Time: " + toString(avgWait) + " seconds", menuPadding);
            
            centerText("Consistency Check: " + string(sys.regularSales == sys.regularWaiterSales ? "PASSED" : "PENDING..."), menuPadding);
            centerText("---------------------------------", menuPadding);
            cout << endl;
            centerText("Press any key to return...", menuPadding);
            system("pause > nul");
            printHeader();
        }
        else if (choice == 3) {
            break;
        }
        else if (choice == 4) {
            printHeader();
            centerText("--- PEAK-HOUR STRESS SIMULATION ---", menuPadding);
            centerText("Spawning 5 simultaneous random orders...", menuPadding);
            
            for(int i=0; i<5; i++) {
                Order* randOrder = new Order();
                randOrder->orderNum = orderCounter++;
                randOrder->customerName = "SimCust_" + toString(i+1);
                randOrder->customerID = "ID-" + toString(100+i);
                randOrder->phone = "555-010" + toString(i);
                randOrder->isVIP = (i % 3 == 0); 
                
                Node<Dish>* d = sys.menu.getHead();
                if(d) {
                    randOrder->dishes.insert(d->data);
                    randOrder->totalBill += d->data.price;
                }
                _beginthread(customerThread, 0, (void*)randOrder);
            }
            cout << endl;
            centerText("[ALERT] 5 Threads started. Logs will overlap.", menuPadding);
            system("pause");
            printHeader();
        }
    }

    return 0;
}
