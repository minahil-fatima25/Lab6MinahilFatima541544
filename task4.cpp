//Task 4,Waiting Room Management System
#include <iostream>
using namespace std;

//array based circular queue of patient ids
class WaitingRoom {
    int* data;
    int capacity;
    int frontIdx;   //array index of the patient who is next in line
    int count;      //number of patients waiting
    int nextId;     //id given to the next patient, never reused

    //double the array when it fills up, keeping the queue order
    void grow() {
        int* bigger = new int[capacity * 2];
        for (int i = 0; i < count; i++)
            bigger[i] = data[(frontIdx + i) % capacity];
        delete[] data;
        data = bigger;
        capacity *= 2;
        frontIdx = 0;
    }

public:
    WaitingRoom(int cap = 5) : capacity(cap), frontIdx(0), count(0), nextId(1) {
        data = new int[capacity];
    }
    ~WaitingRoom() { delete[] data; }
    WaitingRoom(const WaitingRoom&) = delete;
    WaitingRoom& operator=(const WaitingRoom&) = delete;

    //gives the patient the next id and adds them to the back of the queue
    int RegisterPatient() {
        if (count == capacity) grow();
        int id = nextId++;
        data[(frontIdx + count) % capacity] = id;
        count++;
        return id;
    }

    //removes the patient at the front, returns -1 if nobody is waiting
    int ServePatient() {
        if (count == 0) return -1;
        int id = data[frontIdx];
        frontIdx = (frontIdx + 1) % capacity;
        count--;
        return id;
    }

    //empties the queue, ids already given are not reused
    void CancelAll() {
        frontIdx = 0;
        count = 0;
    }

    bool CanDoctorGoHome() const {
        return count == 0;
    }

    //prints waiting ids in ascending order, sorts a copy so the queue is untouched
    void ShowAllPatient() const {
        if (count == 0) {
            cout << "No patients waiting\n";
            return;
        }
        int* temp = new int[count];
        for (int i = 0; i < count; i++)
            temp[i] = data[(frontIdx + i) % capacity];

        //insertion sort
        for (int i = 1; i < count; i++) {
            int key = temp[i];
            int j = i - 1;
            while (j >= 0 && temp[j] > key) {
                temp[j + 1] = temp[j];
                j--;
            }
            temp[j + 1] = key;
        }

        cout << "Waiting patients (" << count << "): ";
        for (int i = 0; i < count; i++) cout << temp[i] << " ";
        cout << "\n";
        delete[] temp;
    }
};

int main() {
    WaitingRoom room;
    int choice;

    while (true) {
        cout << "\n1. Register patient\n2. Serve patient\n3. Cancel all appointments\n"
             << "4. Can doctor go home?\n5. Show all waiting patients\n0. Exit\nChoice: ";
        cin >> choice;
        if (choice == 0) break;

        if (choice == 1) {
            cout << "Patient registered with ID " << room.RegisterPatient() << "\n";
        }
        else if (choice == 2) {
            int id = room.ServePatient();
            if (id == -1) cout << "No patients waiting\n";
            else cout << "Now serving patient " << id << "\n";
        }
        else if (choice == 3) {
            room.CancelAll();
            cout << "All appointments cancelled\n";
        }
        else if (choice == 4) {
            cout << (room.CanDoctorGoHome() ? "Yes, no one is waiting" : "No, patients are still waiting") << "\n";
        }
        else if (choice == 5) {
            room.ShowAllPatient();
        }
        else {
            cout << "Invalid choice\n";
        }
    }
    return 0;
}