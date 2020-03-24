#include "system.h"
#include "git.hpp"


int main() {
	std::cout << "hello " << PROJECT_NAME << std::endl;
	std::cout << "Git CommitID:" << COMMIT_ID << std::endl;
	return 0;
}