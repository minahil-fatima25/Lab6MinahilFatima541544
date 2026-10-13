//Task 1,Expression Conversion and Evaluation
//Own stack implementation(dynamic array),No STL stack/vector is used

#include <iostream>
#include <string>
#include <cmath>
using namespace std;

//Generic array-based Stack,The array doubles when it fills up

template <typename T>
class Stack {
private:
    T*  data;       //the array that holds the items
    int topIndex;   //index of the top item(-1 means empty)
    int capacity;   //how many items the array can hold right now

    void grow() {                       //double the capacity
        int newCap = capacity * 2;
        T* bigger = new T[newCap];
        for (int i = 0; i <= topIndex; i++) bigger[i] = data[i];
        delete[] data;
        data = bigger;
        capacity = newCap;
    }

public:
    Stack(int cap = 10) : topIndex(-1), capacity(cap) { data = new T[capacity]; }
    ~Stack() { delete[] data; }

    //Prevent accidental copying
    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;

    bool isEmpty() const { return topIndex == -1; }

    void push(const T& item) {
        if (topIndex + 1 == capacity) grow();
        data[++topIndex] = item;
    }

    T pop() {
        if (isEmpty()) throw runtime_error("Stack underflow");
        return data[topIndex--];
    }

    T peek() const {
        if (isEmpty()) throw runtime_error("Stack is empty");
        return data[topIndex];
    }
};

// Helper functions
bool isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

bool isOperand(char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

// Higher number = binds tighter
int precedence(char op) {
    if (op == '^') return 3;
    if (op == '*' || op == '/') return 2;
    if (op == '+' || op == '-') return 1;
    return 0;                           // '(' or anything else
}

// Only ^ is right-associative: 2^3^2 means 2^(3^2)
bool isRightAssoc(char op) { return op == '^'; }

string reverseString(const string& s) {
    string r = "";
    for (int i = (int)s.size() - 1; i >= 0; i--) r += s[i];
    return r;
}


//Core shunting-yard routine used by BOTH infix->postfix and infix->prefix.
//'forPrefix' flips the tie-breaking rule (needed because prefix is
//built by reversing the expression).
string convertInfix(const string& expr, bool forPrefix) {
    Stack<char> ops;
    string out = "";

    for (size_t i = 0; i < expr.size(); i++) {
        char c = expr[i];

        if (c == ' ') continue;

        if (isOperand(c)) {
            out += c;                               //operands go straight out
        }
        else if (c == '(') {
            ops.push(c);
        }
        else if (c == ')') {
            while (!ops.isEmpty() && ops.peek() != '(') out += ops.pop();
            if (ops.isEmpty()) throw runtime_error("Mismatched parentheses");
            ops.pop();                              // discard the '('
        }
        else if (isOperator(c)) {
            while (!ops.isEmpty() && ops.peek() != '(') {
                int topP = precedence(ops.peek());
                int curP = precedence(c);
                bool popIt;
                if (!forPrefix) popIt = isRightAssoc(c) ? (topP > curP) : (topP >= curP);
                else            popIt = isRightAssoc(c) ? (topP >= curP) : (topP > curP);
                if (!popIt) break;
                out += ops.pop();
            }
            ops.push(c);
        }
        else {
            throw runtime_error(string("Invalid character: ") + c);
        }
    }

    while (!ops.isEmpty()) {
        char t = ops.pop();
        if (t == '(') throw runtime_error("Mismatched parentheses");
        out += t;
    }
    return out;
}


//1)Infix -> Postfix
string infix_to_postfix(string expr) {
    return convertInfix(expr, false);
}
//2)Infix -> Prefix
string infix_to_prefix(string expr) {
    string rev = reverseString(expr);
    for (size_t i = 0; i < rev.size(); i++) {
        if (rev[i] == '(') rev[i] = ')';
        else if (rev[i] == ')') rev[i] = '(';
    }
    string post = convertInfix(rev, true);
    return reverseString(post);
}

// 3) Postfix -> Infix  (stack of strings)
string postfix_to_infix(string expr) {
    Stack<string> st;
    for (size_t i = 0; i < expr.size(); i++) {
        char c = expr[i];
        if (c == ' ') continue;

        if (isOperand(c)) {
            st.push(string(1, c));
        }
        else if (isOperator(c)) {
            string right = st.pop();                //second operand comes off first
            string left  = st.pop();
            st.push("(" + left + c + right + ")");
        }
        else throw runtime_error(string("Invalid character: ") + c);
    }
    string result = st.pop();
    if (!st.isEmpty()) throw runtime_error("Malformed postfix expression");
    return result;
}

//Apply one operator to two numbers
double applyOp(char op, double a, double b) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/':
            if (b == 0) throw runtime_error("Division by zero");
            return a / b;
        case '^': return pow(a, b);
    }
    throw runtime_error("Unknown operator");
}
// 4) Evaluate Postfix  (scan LEFT -> RIGHT)

double evaluate_postfix(string expr) {
    Stack<double> st;
    for (size_t i = 0; i < expr.size(); i++) {
        char c = expr[i];
        if (c == ' ') continue;

        if (c >= '0' && c <= '9') {
            st.push(c - '0');                       //char '7' -> number 7
        }
        else if (isOperator(c)) {
            double b = st.pop();                    //right operand first
            double a = st.pop();
            st.push(applyOp(c, a, b));
        }
        else throw runtime_error(string("Invalid character: ") + c);
    }
    double result = st.pop();
    if (!st.isEmpty()) throw runtime_error("Malformed postfix expression");
    return result;
}

//5) Evaluate Prefix  (scan RIGHT -> LEFT)
double evaluate_prefix(string expr) {
    Stack<double> st;
    for (int i = (int)expr.size() - 1; i >= 0; i--) {
        char c = expr[i];
        if (c == ' ') continue;

        if (c >= '0' && c <= '9') {
            st.push(c - '0');
        }
        else if (isOperator(c)) {
            double a = st.pop();                    //LEFT operand comes off first here
            double b = st.pop();
            st.push(applyOp(c, a, b));
        }
        else throw runtime_error(string("Invalid character: ") + c);
    }
    double result = st.pop();
    if (!st.isEmpty()) throw runtime_error("Malformed prefix expression");
    return result;
}

// Test driver

int main() {
    int choice;
    string expr;

    do {
        cout << "\n===== Expression Converter =====\n"
             << "1. Infix   -> Postfix\n"
             << "2. Infix   -> Prefix\n"
             << "3. Postfix -> Infix\n"
             << "4. Evaluate Postfix\n"
             << "5. Evaluate Prefix\n"
             << "0. Exit\n"
             << "Choice: ";
        cin >> choice;
        if (choice == 0) break;

        cout << "Enter expression (no spaces needed): ";
        cin >> expr;

        try {
            switch (choice) {
                case 1: cout << "Postfix: " << infix_to_postfix(expr) << endl; break;
                case 2: cout << "Prefix : " << infix_to_prefix(expr) << endl; break;
                case 3: cout << "Infix  : " << postfix_to_infix(expr) << endl; break;
                case 4: cout << "Result : " << evaluate_postfix(expr) << endl; break;
                case 5: cout << "Result : " << evaluate_prefix(expr) << endl; break;
                default: cout << "Invalid choice.\n";
            }
        } catch (exception& e) {
            cout << "Error: " << e.what() << endl;
        }
    } while (true);

    return 0;
}