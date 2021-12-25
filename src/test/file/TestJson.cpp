#define __MT_TEST_JSON_CPP__

#include <iostream>

#include <nlohmann/json.hpp>

#include "common.h"
#include "TestJson.h"

using namespace std;
using json = nlohmann::json;
using namespace mt::test;


TestJson::TestJson()
{

}

TestJson::~TestJson()
{

}

void TestJson::run()
{
	// jdEmployees
    json jdEmployees =
    {
        {"firstName","Sean"},
        {"lastName","Brown"},
        {"StudentID",21453},
        {"Department","Computer Sc."}
    };

    // Access the values
    string fName = jdEmployees.value("firstName", "oops");
    string lName = jdEmployees.value("lastName", "oops");
    int sID = jdEmployees.value("StudentID", 0);
    string dept = jdEmployees.value("Department", "oops");
   
    // Print the values
    cout << "First Name: " << fName << endl;
    cout << "Last Name: " << lName << endl;
    cout << "Student ID: " << sID << endl;
    cout << "Department: " << dept << endl;
}
