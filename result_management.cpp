/*
 * OOP Using C++ - Task 2 (Weeks 7-13)
 * Topic : Student Result Management System
 * Concepts: Inheritance, Virtual Functions (Runtime Polymorphism),
 *           Exception Handling, File Handling, Templates,
 *           STL vector, STL Algorithms and Lambda Expressions
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <exception>
#include <stdexcept>
#include <iomanip>
using namespace std;

// ---------- Custom exception for invalid marks ----------
class InvalidMarksException : public exception {
    string msg;
public:
    InvalidMarksException(const string &subject, int value) {
        msg = "Invalid marks " + to_string(value) + " for " + subject +
              " (allowed range: 0 - 100)";
    }
    const char *what() const noexcept override { return msg.c_str(); }
};

// ---------- Base class ----------
class Student {
protected:
    int    rollNo;
    string name;
public:
    Student(int r, const string &n) : rollNo(r), name(n) {}
    virtual ~Student() {}                         // virtual destructor

    int           getRollNo() const { return rollNo; }
    const string &getName()   const { return name; }

    // Virtual functions (overridden in Result)
    virtual void display() const {                // student details
        cout << "Roll No: " << rollNo << " | Name: " << name << endl;
    }
    virtual void displayMarks() const {           // marks (none in base class)
        cout << "   (No marks recorded)" << endl;
    }
    virtual int    getTotal() const { return 0; }
    virtual string toRecord() const {             // one line for the file
        return to_string(rollNo) + "," + name;
    }
};

// ---------- Derived class ----------
class Result : public Student {
public:
    static const int SUBJECTS = 3;
private:
    int marks[SUBJECTS];
    static const char *subjectName(int i) {
        static const char *names[SUBJECTS] = {"Mathematics", "Electronics", "C++ Programming"};
        return names[i];
    }
public:
    Result(int r, const string &n, int m1, int m2, int m3) : Student(r, n) {
        int m[SUBJECTS] = {m1, m2, m3};
        for (int i = 0; i < SUBJECTS; ++i) {
            if (m[i] < 0 || m[i] > 100)
                throw InvalidMarksException(subjectName(i), m[i]);   // validation
            marks[i] = m[i];
        }
    }

    int    getTotal()      const override { return marks[0] + marks[1] + marks[2]; }
    double getPercentage() const { return getTotal() * 100.0 / (SUBJECTS * 100); }
    bool   isPass()        const {
        return all_of(marks, marks + SUBJECTS, [](int m) { return m >= 40; });
    }
    char getGrade() const {
        double p = getPercentage();
        if (!isPass()) return 'F';
        if (p >= 90) return 'A';
        if (p >= 75) return 'B';
        if (p >= 60) return 'C';
        return 'D';
    }

    void display() const override {
        Student::display();
    }
    void displayMarks() const override {
        cout << "   Marks : ";
        for (int i = 0; i < SUBJECTS; ++i)
            cout << subjectName(i) << "=" << marks[i] << "  ";
        cout << endl;
        cout << "   Total : " << getTotal() << "/" << SUBJECTS * 100
             << " | Percentage: " << fixed << setprecision(2) << getPercentage()
             << "% | Grade: " << getGrade()
             << " | Result: " << (isPass() ? "PASS" : "FAIL") << endl;
    }
    string toRecord() const override {
        ostringstream os;
        os << rollNo << "," << name;
        for (int i = 0; i < SUBJECTS; ++i) os << "," << marks[i];
        return os.str();
    }
};

typedef vector<shared_ptr<Student>> StudentList;

// ---------- Template function ----------
template <typename T>
T maxOf(const vector<T> &v) {
    return *max_element(v.begin(), v.end());
}

// ---------- Helpers ----------
void addStudent(StudentList &list, int roll, const string &name, int m1, int m2, int m3) {
    try {
        list.push_back(make_shared<Result>(roll, name, m1, m2, m3));
        cout << "  Added  : " << roll << " " << name << endl;
    } catch (const InvalidMarksException &e) {
        cout << "  Rejected (" << roll << " " << name << ") -> " << e.what() << endl;
    }
}

void saveToFile(const StudentList &list, const string &file) {
    ofstream out(file);
    if (!out) throw runtime_error("Cannot open file '" + file + "' for writing");
    for (const auto &s : list) out << s->toRecord() << "\n";
}

StudentList loadFromFile(const string &file) {
    ifstream in(file);
    if (!in) throw runtime_error("Cannot open file '" + file + "' for reading");
    StudentList list;
    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string roll, name, a, b, c;
        getline(ss, roll, ',');  getline(ss, name, ',');
        getline(ss, a, ',');     getline(ss, b, ',');   getline(ss, c, ',');
        try {
            list.push_back(make_shared<Result>(stoi(roll), name, stoi(a), stoi(b), stoi(c)));
        } catch (const exception &e) {
            cout << "  Skipped bad record '" << line << "' : " << e.what() << endl;
        }
    }
    return list;
}

void showAll(const StudentList &list) {
    for (const auto &s : list) {          // runtime polymorphism through base pointer
        s->display();
        s->displayMarks();
    }
}

int main() {
    const string FILE_NAME = "students.txt";
    StudentList students;

    cout << "========== STUDENT RESULT MANAGEMENT SYSTEM ==========\n\n";

    cout << "--- 1. Entering records (marks are validated) ---\n";
    addStudent(students, 101, "Arun",    78, 85, 90);
    addStudent(students, 102, "Meena",   92, 95, 98);
    addStudent(students, 103, "Karthik", 35, 60, 55);
    addStudent(students, 104, "Divya",   65, 70, 58);
    addStudent(students, 105, "Rahul",  105, 80, 75);     // invalid  -> exception
    addStudent(students, 106, "Sneha",   88, -4, 72);     // invalid  -> exception
    addStudent(students, 107, "Vikram",  72, 68, 80);
    cout << "Valid records stored in vector = " << students.size() << "\n\n";

    cout << "--- 2. Student details, total marks and results (virtual functions) ---\n";
    showAll(students);
    cout << endl;

    cout << "--- 3. Saving records to file ---\n";
    try {
        saveToFile(students, FILE_NAME);
        cout << "  " << students.size() << " records saved to '" << FILE_NAME << "'\n\n";
    } catch (const exception &e) {
        cout << "  File error: " << e.what() << "\n\n";
    }

    cout << "--- 4. Retrieving records from file ---\n";
    StudentList loaded;
    try {
        loaded = loadFromFile(FILE_NAME);
        cout << "  " << loaded.size() << " records retrieved from '" << FILE_NAME << "'\n";
        showAll(loaded);
        cout << "\n  Trying to open a file that does not exist:\n";
        loadFromFile("missing_file.txt");
    } catch (const exception &e) {
        cout << "  File error caught: " << e.what() << "\n\n";
    }

    cout << "--- 5. STL algorithms and lambda expressions ---\n";
    int threshold = 225;                                 // specified mark (out of 300)
    StudentList above;
    copy_if(loaded.begin(), loaded.end(), back_inserter(above),
            [threshold](const shared_ptr<Student> &s) { return s->getTotal() > threshold; });
    cout << "Students scoring above " << threshold << " marks (copy_if + lambda):\n";
    for (const auto &s : above)
        cout << "   " << s->getRollNo() << " " << left << setw(8) << s->getName()
             << " Total = " << s->getTotal() << endl;

    long passCount = count_if(loaded.begin(), loaded.end(), [](const shared_ptr<Student> &s) {
        const Result *r = dynamic_cast<const Result *>(s.get());
        return r && r->isPass();
    });
    cout << "\nPassed = " << passCount << " | Failed = " << loaded.size() - passCount
         << " (count_if + lambda)\n";

    StudentList ranked = loaded;
    sort(ranked.begin(), ranked.end(), [](const shared_ptr<Student> &a, const shared_ptr<Student> &b) {
        return a->getTotal() > b->getTotal();
    });
    cout << "\nRank list (sort + lambda):\n";
    int rank = 1;
    for (const auto &s : ranked)
        cout << "   " << rank++ << ". " << left << setw(8) << s->getName()
             << " Total = " << s->getTotal() << endl;

    vector<int> totals(loaded.size());
    transform(loaded.begin(), loaded.end(), totals.begin(),
              [](const shared_ptr<Student> &s) { return s->getTotal(); });
    cout << "\nHighest total (template maxOf<int>) = " << maxOf(totals) << endl;

    cout << "\n========== END OF PROGRAM ==========\n";
    return 0;
}
