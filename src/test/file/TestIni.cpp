#define __MT_TEST_INI_CPP__

#include <iostream>

#include <mini/ini.h>

#include "common.h"
#include "TestIni.h"

using namespace std;
using namespace mINI;
using namespace mt::test;


TestIni::TestIni()
{

}

TestIni::~TestIni()
{

}

void TestIni::run()
{
	// first, create a file instance
	INIFile file("../res/test/file.ini");

	// next, create a structure that will hold data
	INIStructure ini;

	// now we can read the file
	file.read(ini);

	// read a value
	string& amountOfApples = ini["fruits"]["apples"];

	// update a value
	ini["fruits"]["oranges"] = "50";

	// add a new entry
	ini["fruits"]["bananas"] = "100";

	// write updates to file
	file.write(ini);
}
