#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

namespace mt {
namespace engine {

class Exception {

public:

	// Forward
	Exception();
	Exception(std::string msg, std::string file, int line);
	~Exception();

	// Native
	std::string getMessage();
	void setMessage(std::string msg, std::string file, int line);

	// Stock

private:

	// Variable
	std::string message;
	std::string file;
	int line;

};

}}

#endif
