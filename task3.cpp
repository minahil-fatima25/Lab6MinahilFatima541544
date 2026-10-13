//Task 3,Print Server Request Handling System

#include <iostream>
#include <iomanip>
#include <stdexcept>
using namespace std;

//one print job
struct Request {
    int id;
    int arrival;      //time the request arrived
    int processing;   //time the printer needs for it
};

//circular queue that grows when full and shrinks when mostly empty
class CircularQueue {
    Request* data;
    int capacity;
    int frontIdx;      //array index of the first request
    int count;         //how many requests are stored
    int minCapacity;   //never shrink below the starting size

public:
    CircularQueue(int cap = 4) : capacity(cap), frontIdx(0), count(0), minCapacity(cap) {
        data = new Request[capacity];
    }
    ~CircularQueue() { delete[] data; }
    CircularQueue(const CircularQueue&) = delete;
    CircularQueue& operator=(const CircularQueue&) = delete;

    bool isEmpty() const { return count == 0; }
    bool isFull() const { return count == capacity; }
    int size() const { return count; }

    //copy requests in order into a new array so front becomes index 0 again
    void resize(int newCap) {
        if (newCap < count) throw runtime_error("New capacity too small");
        Request* bigger = new Request[newCap];
        for (int i = 0; i < count; i++)
            bigger[i] = data[(frontIdx + i) % capacity];
        delete[] data;
        data = bigger;
        capacity = newCap;
        frontIdx = 0;
        cout << "  (resized to capacity " << capacity << ")\n";
    }

    void enqueue(Request x) {
        if (isFull()) resize(capacity * 2);
        data[(frontIdx + count) % capacity] = x;   //modulo wraps around the array end
        count++;
    }

    Request dequeue() {
        if (isEmpty()) throw runtime_error("Queue is empty");
        Request x = data[frontIdx];
        frontIdx = (frontIdx + 1) % capacity;
        count--;
        //too sparse: a quarter full or less, halve the capacity
        if (capacity > minCapacity && count <= capacity / 4)
            resize(max(minCapacity, capacity / 2));
        return x;
    }

    Request front() const {
        if (isEmpty()) throw runtime_error("Queue is empty");
        return data[frontIdx];
    }

    //prints the requests in order and the raw array slots
    void display() const {
        cout << "  Queue: ";
        if (isEmpty()) cout << "empty";
        for (int i = 0; i < count; i++) {
            Request r = data[(frontIdx + i) % capacity];
            cout << "[#" << r.id << " a=" << r.arrival << " p=" << r.processing << "] ";
        }
        int rear = isEmpty() ? -1 : (frontIdx + count - 1) % capacity;
        cout << "\n  size=" << count << " capacity=" << capacity
             << " front=" << frontIdx << " rear=" << rear << "\n  Slots: ";
        for (int i = 0; i < capacity; i++) {
            int offset = (i - frontIdx + capacity) % capacity;
            if (offset < count) cout << "[#" << data[i].id << "] ";
            else cout << "[_] ";
        }
        cout << "\n";
    }
};

//running totals for the printer
struct Stats {
    int clock = 0;       //time the printer becomes free
    int totalWait = 0;
    int processed = 0;
};

void addRequest(CircularQueue& q, int id, int arrival, int processing) {
    Request r = {id, arrival, processing};
    cout << "Enqueue request #" << id << "\n";
    q.enqueue(r);
    q.display();
}

//printer takes the front request, waiting time = start time - arrival time
void processNext(CircularQueue& q, Stats& st) {
    if (q.isEmpty()) {
        cout << "No requests to process\n";
        return;
    }
    cout << "Dequeue\n";
    Request r = q.dequeue();
    int start = max(st.clock, r.arrival);   //printer may be idle until the request arrives
    int wait = start - r.arrival;
    st.clock = start + r.processing;
    st.totalWait += wait;
    st.processed++;
    cout << "  Processed #" << r.id << ": starts at " << start
         << ", waited " << wait << ", finishes at " << st.clock << "\n";
    q.display();
}

void showAverage(const Stats& st) {
    if (st.processed == 0) {
        cout << "No requests processed yet\n";
        return;
    }
    cout << "Processed " << st.processed << " requests, average waiting time = "
         << fixed << setprecision(2) << (double)st.totalWait / st.processed << "\n";
}

//fixed scenario that shows wrap around, growing and shrinking
void runDemo() {
    CircularQueue q(4);
    Stats st;
    addRequest(q, 1, 0, 5);
    addRequest(q, 2, 1, 3);
    addRequest(q, 3, 2, 4);
    processNext(q, st);
    addRequest(q, 4, 3, 2);
    addRequest(q, 5, 4, 6);   //wraps to the start of the array
    addRequest(q, 6, 5, 1);   //queue is full, so it grows
    processNext(q, st);
    processNext(q, st);
    processNext(q, st);       //sparse now, so it shrinks
    processNext(q, st);
    processNext(q, st);
    showAverage(st);
}

int main() {
    CircularQueue q(4);
    Stats st;
    int nextId = 1;
    int choice;

    while (true) {
        cout << "\n1. Add request\n2. Process next request\n3. Show front request\n"
             << "4. Show queue\n5. Average waiting time\n6. Run demo\n0. Exit\nChoice: ";
        cin >> choice;
        if (choice == 0) break;

        if (choice == 1) {
            int arrival, processing;
            cout << "Arrival time: ";
            cin >> arrival;
            cout << "Processing time: ";
            cin >> processing;
            if (arrival < 0 || processing <= 0) {
                cout << "Arrival must be 0 or more and processing time above 0\n";
                continue;
            }
            addRequest(q, nextId++, arrival, processing);
        }
        else if (choice == 2) processNext(q, st);
        else if (choice == 3) {
            if (q.isEmpty()) cout << "Queue is empty\n";
            else {
                Request r = q.front();
                cout << "Front: #" << r.id << " arrival=" << r.arrival
                     << " processing=" << r.processing << "\n";
            }
        }
        else if (choice == 4) q.display();
        else if (choice == 5) showAverage(st);
        else if (choice == 6) runDemo();
        else cout << "Invalid choice\n";
    }
    return 0;
}