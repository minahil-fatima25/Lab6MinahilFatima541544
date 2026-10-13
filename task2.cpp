//Task 2,Expression Validators Using Stack

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
using namespace std;

//array based stack, doubles its size when full
template <typename T>
class Stack {
    T* data;
    int topIndex;   //index of the top item, -1 when empty
    int capacity;

    //copy everything into a bigger array
    void grow() {
        T* bigger = new T[capacity * 2];
        for (int i = 0; i <= topIndex; i++) bigger[i] = data[i];
        delete[] data;
        data = bigger;
        capacity *= 2;
    }

public:
    Stack(int cap = 10) : topIndex(-1), capacity(cap) { data = new T[capacity]; }
    ~Stack() { delete[] data; }
    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;

    bool isEmpty() const { return topIndex == -1; }

    void push(const T& item) {
        if (topIndex + 1 == capacity) grow();
        data[++topIndex] = item;
    }

    //remove and return the top item
    T pop() {
        if (isEmpty()) throw runtime_error("Stack underflow");
        return data[topIndex--];
    }

    //look at the top item without removing it
    T peek() const {
        if (isEmpty()) throw runtime_error("Stack is empty");
        return data[topIndex];
    }
};

//part A: checks that (), {} and [] are balanced and correctly nested
bool isValidBrackets(string s) {
    Stack<char> st;
    for (size_t i = 0; i < s.size(); i++) {
        char c = s[i];

        //opening bracket, wait for its closer
        if (c == '(' || c == '{' || c == '[') {
            st.push(c);
        }
        //closing bracket, must match the most recent opener
        else if (c == ')' || c == '}' || c == ']') {
            if (st.isEmpty()) return false;
            char open = st.pop();
            if ((c == ')' && open != '(') ||
                (c == '}' && open != '{') ||
                (c == ']' && open != '[')) return false;
        }
    }
    //anything left on the stack was never closed
    return st.isEmpty();
}

//part B: checks tag nesting, returns (isValid, list of errors)
pair<bool, vector<string> > validate_markup(string s) {
    Stack<string> st;       //names of tags that are currently open
    vector<string> errors;
    size_t i = 0;

    while (i < s.size()) {
        if (s[i] != '<') { i++; continue; }

        //find where this tag ends
        size_t end = s.find('>', i);
        if (end == string::npos) {
            errors.push_back("Tag starting at position " + to_string(i) + " is missing '>'");
            break;
        }

        //text between < and >
        string inside = s.substr(i + 1, end - i - 1);
        i = end + 1;

        if (inside.empty()) {
            errors.push_back("Empty tag <>");
            continue;
        }

        //skip doctype and xml declarations
        if (inside[0] == '!' || inside[0] == '?') continue;

        bool closing = (inside[0] == '/');
        bool selfClosing = (inside[inside.size() - 1] == '/');

        //extract the tag name, stops at a space or slash so attributes are ignored
        size_t start = closing ? 1 : 0;
        string name = "";
        while (start < inside.size() && inside[start] != ' ' && inside[start] != '/') {
            name += inside[start];
            start++;
        }

        if (name.empty()) {
            errors.push_back("Tag without a name");
            continue;
        }

        //self closing tags like <br/> never need a closer
        if (selfClosing && !closing) continue;

        if (!closing) {
            st.push(name);
        }
        else if (st.isEmpty()) {
            errors.push_back("Closing tag </" + name + "> has no opening tag");
        }
        else if (st.peek() != name) {
            //wrong order, report it and drop the tag it was expecting
            errors.push_back("Expected </" + st.peek() + "> but found </" + name + ">");
            st.pop();
        }
        else {
            st.pop();
        }
    }

    //tags still open at the end were never closed
    while (!st.isEmpty()) {
        errors.push_back("Tag <" + st.pop() + "> is never closed");
    }

    return make_pair(errors.empty(), errors);
}

//menu driven test program
int main() {
    int choice;
    string s;

    while (true) {
        cout << "\n1. Validate brackets\n2. Validate HTML/XML\n0. Exit\nChoice: ";
        cin >> choice;
        if (choice == 0) break;
        cin.ignore();   //clear the newline so getline works

        if (choice == 1) {
            cout << "Enter string: ";
            getline(cin, s);
            cout << (isValidBrackets(s) ? "Valid" : "Invalid") << endl;
        }
        else if (choice == 2) {
            cout << "Enter markup: ";
            getline(cin, s);
            pair<bool, vector<string> > r = validate_markup(s);
            if (r.first) {
                cout << "Valid" << endl;
            } else {
                cout << "Invalid" << endl;
                for (size_t i = 0; i < r.second.size(); i++)
                    cout << "  " << i + 1 << ". " << r.second[i] << endl;
            }
        }
        else {
            cout << "Invalid choice" << endl;
        }
    }
    return 0;
}