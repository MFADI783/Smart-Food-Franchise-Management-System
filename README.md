<div align="center">

# 🍔 Smart Food Franchise Management System

### *A High-Performance Multithreaded Simulation Built in C++*

---

![Language](https://img.shields.io/badge/Language-C%2B%2B-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)
![Platform](https://img.shields.io/badge/Platform-Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white)
![IDE](https://img.shields.io/badge/IDE-Dev--C%2B%2B%20%7C%20Visual%20Studio-5C2D91?style=for-the-badge&logo=visualstudio&logoColor=white)
![Threading](https://img.shields.io/badge/Multithreading-Win32%20API-red?style=for-the-badge)

</div>

---

## 📖 Overview

**Smart Food Franchise Management System** is a fully concurrent, multithreaded C++ application that simulates the real-world operations of a food franchise. From the moment a customer places an order to the final billing, every stage is handled by dedicated threads — mimicking how an actual restaurant coordinates between its **Manager**, **Kitchen Cooks**, and **Waiters** simultaneously.

This project showcases advanced operating systems concepts including **thread synchronization**, **inter-process communication via pipes**, **priority queues**, **critical sections**, and **custom data structures** — all implemented from scratch without relying on STL containers.

---

## ✨ Key Features

| Feature | Description |
|---|---|
| 🧵 **Multithreaded Architecture** | Separate threads for Manager, Cooks, and Waiters running concurrently |
| 👑 **VIP Priority Handling** | VIP customers jump the queue using a priority-based order system |
| 🔗 **Pipe-Based Communication** | Customer → Manager communication via Win32 named pipes |
| 🔒 **Thread-Safe Data Structures** | Custom `LinkedList` and `ThreadSafeQueue` with `CRITICAL_SECTION` guards |
| 📋 **Dynamic Menu Loading** | Menu loaded at runtime from `menu.txt` (easily configurable) |
| 💰 **Real-Time Billing** | Automatic bill calculation with per-waiter sales tracking |
| 📊 **Performance Metrics** | Tracks total wait time, customers served, and revenue per session |
| 🎯 **Order Splitting** | Large orders split into individual dish tasks for parallel kitchen processing |

---

## 🏗️ System Architecture

```
┌─────────────────────────────────────────────────────────┐
│                    CUSTOMER THREAD                       │
│          Places orders → Writes to Pipe                 │
└────────────────────────┬────────────────────────────────┘
                         │ (Win32 Pipe)
                         ▼
┌─────────────────────────────────────────────────────────┐
│                    MANAGER THREAD                        │
│       Reads pipe → Assigns to Order Queue               │
└────────────────────────┬────────────────────────────────┘
                         │ (ThreadSafeQueue<Order*>)
                         ▼
┌─────────────────────────────────────────────────────────┐
│               KITCHEN QUEUE (Cook Threads)               │
│     Dish tasks processed in parallel by multiple cooks  │
└────────────────────────┬────────────────────────────────┘
                         │ (ThreadSafeQueue<Order*>)
                         ▼
┌─────────────────────────────────────────────────────────┐
│                    WAITER THREADS                        │
│         Deliver completed orders → Update billing       │
└─────────────────────────────────────────────────────────┘
```

---

## 📁 Project Structure

```
Smart-Food-Franchise-System/
│
├── 📄 main.cpp                # Entry point — spawns all threads & initializes system
├── 📄 system_logic.cpp        # Core RestaurantSystem class — all thread functions
├── 📄 models.h                # Data models: Dish, Order, DishTask, SalesRecord
├── 📄 CustomStructures.h      # Custom LinkedList & ThreadSafeQueue implementations
├── 📄 menu.txt                # Menu configuration file (Name, ID, Price, PrepTime)
├── 📄 smartfood.dev           # Dev-C++ project file
└── 📄 README.md               # You are here
```

---

## 🛠️ Technologies & Concepts

- **Language:** C++ (compatible with older compiler standards)
- **Threading:** Win32 `_beginthread` / `CreateThread`
- **Synchronization:** `CRITICAL_SECTION`, `EnterCriticalSection`, `LeaveCriticalSection`
- **IPC:** Win32 `CreatePipe` / `ReadFile` / `WriteFile`
- **Custom DSA:** Hand-built `LinkedList<T>` and `ThreadSafeQueue<T>` (no STL)
- **Console UI:** Centered text rendering with 80-column formatting

---

## 📦 System Modules

### 🧾 Order Management
Handles customer input, order object creation, VIP flagging, and pipe-based forwarding to the manager thread.

### 🍳 Kitchen Processing
Decomposes each order into individual `DishTask` objects. Cook threads pull from the kitchen queue and simulate preparation time using `Sleep()`.

### 🚚 Delivery & Billing
Waiter threads pick up fully prepared orders, calculate the total bill, log sales records, and update the global metrics.

### 📈 Performance Reporting
At session end, a summary is printed showing: total customers served, total revenue, average wait time, and per-waiter sales breakdown.

---

## ▶️ How to Run

### Prerequisites
- Windows OS (required for Win32 threading APIs)
- Dev-C++ **or** Visual Studio (any recent version)

### Steps

```bash
# Option 1: Dev-C++
1. Open smartfood.dev in Dev-C++
2. Press F9 (Compile & Run)

# Option 2: Visual Studio
1. Create a new Empty C++ Project
2. Add all .cpp and .h files
3. Set menu.txt in the project root
4. Build and Run (Ctrl + F5)
```

> ⚠️ **Important:** Make sure `menu.txt` is in the **same directory** as the executable, otherwise the menu will fail to load.

---

## 🍽️ Menu Configuration

The `menu.txt` file follows a simple CSV format. You can add or modify dishes easily:

```
DishName, ID, Price(PKR), PrepTime(seconds)
```

**Default Menu:**
| Dish | Price | Prep Time |
|---|---|---|
| Burger | Rs. 450 | 5 sec |
| Pizza | Rs. 1200 | 8 sec |
| Sandwich | Rs. 350 | 3 sec |
| Pasta | Rs. 600 | 6 sec |
| Fries | Rs. 200 | 2 sec |
| Coke | Rs. 100 | 1 sec |

---

## 👨‍💻 Author

**Hassan Bukhari**  
*Systems & Concurrent Programming — C++ Multithreaded Project*

---

<div align="center">

*Built with ❤️ and lots of threads*

</div>
