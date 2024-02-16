//  main.cpp
//  CS300 Project 2
//  Course Panner
//  Created by Hannah Hendrix on 8/10/23.

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;


class Course {
public:
    int courseNumber;
    string  courseName;
    vector<string> PreReqs;
    string departmentPrefix;
};

class Node {
public:
    Course data;
    Node* left;
    Node* right;
};

//Function to insert a Course into the Binary Tree
void InsertNode(Node*& root, Course course) {
    if (root == nullptr) {
        Node* newNode = new Node;
        newNode->data = course;
        newNode->left = nullptr;
        newNode->right = nullptr;
        root = newNode;
    } else if (course.courseNumber < root->data.courseNumber) {
        InsertNode(root->left, course);
    } else if (course.courseNumber > root->data.courseNumber) {
        InsertNode(root->right, course);
    } else {
        if (course.departmentPrefix < root->data.departmentPrefix) {
            InsertNode(root->left, course);
        } else {
            InsertNode(root->right, course);
        }
    }
}

//Function to search for a Course in the Binary Tree
Course SearchCourse(Node* root, const string& courseNum) {
    if (root == nullptr) {
        Course notFound;
        notFound.courseNumber = 0;
        return notFound;
    }

    string fullCourseNum = root->data.departmentPrefix + to_string(root->data.courseNumber);

    if (fullCourseNum == courseNum) {
        return root->data;
    } else if (courseNum < fullCourseNum) {
        return SearchCourse(root->left, courseNum);
    } else {
        return SearchCourse(root->right, courseNum);
    }
}


//Function to open, read, and parse the file
vector<string> OpenReadFile(const string& filename) {
    fs::path filePath = fs::current_path() / filename;
    ifstream file(filePath, ios::binary);

    if (file.is_open()) {
        vector<string> information;
        string line;
        while (getline(file, line)) {
            information.push_back(line);
        }
        file.close();
        return information;
    } else {
        cout << "Error: Unable to open file" << endl;
        return vector<string>();
    }
}
//Function to create Course objects from file data
vector<Course> CreateCourseObjects(const vector<string>& fileData) {
    vector<Course> courses;
    for (const string& line : fileData) {
        vector<string> tokens;
        istringstream iss(line);
        string token;
        while (getline(iss, token, ',')) {
            tokens.push_back(token);
        }

        if (tokens.size() < 2) {
            cout << "Invalid data format: " << line << endl;
            continue;
        }

        Course course;

        // Extract department prefix and course number
        string courseNumberStr = tokens[0];
        size_t numberStart = courseNumberStr.find_first_of("0123456789");
        if (numberStart != string::npos) {
            course.departmentPrefix = courseNumberStr.substr(0, numberStart);
            string numericPart = courseNumberStr.substr(numberStart);
            try {
                course.courseNumber = stoi(numericPart);
            } catch (const invalid_argument&) {
                cout << "Invalid course number format: " << line << endl;
                continue;
            }
        } else {
            cout << "Invalid course number format: " << line << endl;
            continue;
        }

        course.courseName = tokens[1];
        for (size_t i = 2; i < tokens.size(); ++i) {
            course.PreReqs.push_back(tokens[i]);
        }
        courses.push_back(course);
    }
    return courses;
}

//Function to print Course information and PreReqs
void PrintCourseInformation(const Course& course) {
    cout << course.departmentPrefix << course.courseNumber << ", " << course.courseName << endl;
    cout << "Prerequisites: ";
    for (const string& prerequisite : course.PreReqs) {
        cout << prerequisite << " ";
    }
    cout << endl;
}

//Function to display the menu
void ShowMenu() {
    cout << "Welcome to the course planner." << endl;
    cout << "\t1. Load Data Structure." << endl;
    cout << "\t2. Print Course List." << endl;
    cout << "\t3. Print Course." << endl;
    cout << "\t9. Exit" << endl;
    cout << "What would you like to do?";
}

void SortAndPrintAlphanumericList(const vector<Course>& courses) {
    vector<Course> sortedCourses = courses;
    sort(sortedCourses.begin(), sortedCourses.end(), [](const Course& a, const Course& b) {
        if (a.departmentPrefix != b.departmentPrefix) {
            return a.departmentPrefix < b.departmentPrefix;
        }
        return a.courseNumber < b.courseNumber;
    });

    cout << "Here is the sample schedule:" << endl;
    for (const Course& course : sortedCourses) {
        cout << course.departmentPrefix << course.courseNumber << ", " << course.courseName << endl;
    }
}


int main(int argc, char* argv[]) {
    bool dataLoaded = false;
    vector<Course> courses;
    Node* root = nullptr;
    
    while (true) {
        ShowMenu();
        int choice;
        cin >> choice;
        cin.ignore(); //consume newline character
        
        switch (choice) {
            case 1:
                if (!dataLoaded) {
                    string filename;
                    cout << "Enter the path to the input file: ";
                    getline(cin, filename);
                    
                    vector<string> fileData = OpenReadFile(filename);
                    if (!fileData.empty()) {
                        courses = CreateCourseObjects(fileData);
                        for (const Course& course : courses) {
                            InsertNode(root, course);
                        }
                        dataLoaded = true;
                        cout << "Data loaded successfully" << endl;
                    }
                    else {
                        cout << "Data is already loaded" << endl;
                    }
                }
                break;
                
            case 2:
                if (dataLoaded) {
                    SortAndPrintAlphanumericList(courses);
                }
                else {
                    cout << "Data is not yet loaded" << endl;
                }
                break;
                
            case 3:
                if (dataLoaded) {
                    string courseNumberStr;
                    cout << "What course do you want to know about?" << endl;
                    cin >> courseNumberStr;

                    Course foundCourse = SearchCourse(root, courseNumberStr);
                    if (foundCourse.courseNumber != 0) {
                        PrintCourseInformation(foundCourse);
                    }
                    else {
                        cout << "Course not found" << endl;
                    }
                }
                else {
                    cout << "Data is not yet loaded" << endl;
                }
                break;
                
            case 9:
                cout << "Thank you for using the course planner!" << endl;
                return 0;
                
            default:
                cout << choice << " is not a valid option." << endl;
        }
    }
    
    return 0;
}

