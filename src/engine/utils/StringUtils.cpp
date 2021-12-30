#define __MT_STRING_UTILS_CPP__

#include <sstream>

#include "common.h"
#include "StringUtils.h"

using namespace std;
using namespace mt::engine;

int StringUtils::parseInt(std::string str)
{
	int num;
	stringstream ss;
	ss << str;
	ss >> num;
	return num;
}
