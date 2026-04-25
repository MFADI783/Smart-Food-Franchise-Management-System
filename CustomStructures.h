#ifndef CUSTOM_STRUCTURES_H
#define CUSTOM_STRUCTURES_H

#include <iostream>
#include <string>
#include <windows.h>

// Basic Node for LinkedList and Queue
template <typename T>
struct Node {
    T data;
    Node* next;
    Node(const T& val) : data(val), next(nullptr) {}
};

// Custom LinkedList for Menu and general storage
template <typename T>
class LinkedList {
private:
    Node<T>* head;
    int count;

public:
    LinkedList() : head(nullptr), count(0) {}
    
    // Copy Constructor for Deep Copy
    LinkedList(const LinkedList& other) : head(nullptr), count(0) {
        Node<T>* temp = other.head;
        while (temp) {
            insert(temp->data);
            temp = temp->next;
        }
    }

    // Assignment Operator for Deep Copy
    LinkedList& operator=(const LinkedList& other) {
        if (this != &other) {
            // Clear existing
            this->~LinkedList();
            head = nullptr;
            count = 0;
            Node<T>* temp = other.head;
            while (temp) {
                insert(temp->data);
                temp = temp->next;
            }
        }
        return *this;
    }

    void insert(const T& val) {
        Node<T>* newNode = new Node<T>(val);
        if (!head) {
            head = newNode;
        } else {
            Node<T>* temp = head;
            while (temp->next) temp = temp->next;
            temp->next = newNode;
        }
        count++;
    }

    Node<T>* getHead() { return head; }
    int size() { return count; }

    ~LinkedList() {
        Node<T>* current = head;
        while (current) {
            Node<T>* next = current->next;
            delete current;
            current = next;
        }
    }
};

// Custom Thread-Safe Queue using Mutex (CRITICAL_SECTION)
template <typename T>
class ThreadSafeQueue {
private:
    Node<T>* front;
    Node<T>* rear;
    CRITICAL_SECTION cs;

public:
    ThreadSafeQueue() : front(nullptr), rear(nullptr) {
        InitializeCriticalSection(&cs);
    }

    void enqueue(const T& val, bool priority = false) {
        EnterCriticalSection(&cs);
        Node<T>* newNode = new Node<T>(val);
        if (!rear) {
            front = rear = newNode;
        } else if (priority) {
            // VIP Priority: Insert at Head
            newNode->next = front;
            front = newNode;
        } else {
            rear->next = newNode;
            rear = newNode;
        }
        LeaveCriticalSection(&cs);
    }

    bool dequeue(T& result) {
        EnterCriticalSection(&cs);
        if (!front) {
            LeaveCriticalSection(&cs);
            return false;
        }
        Node<T>* temp = front;
        result = front->data;
        front = front->next;
        if (!front) rear = nullptr;
        delete temp;
        LeaveCriticalSection(&cs);
        return true;
    }

    bool isEmpty() {
        return front == nullptr;
    }

    ~ThreadSafeQueue() {
        T temp;
        while (dequeue(temp));
        DeleteCriticalSection(&cs);
    }
};

#endif
