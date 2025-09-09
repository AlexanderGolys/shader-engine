#include "hyperbolicTests.hpp"
#include "discreteFuncTests.hpp"
#include "formattersTests.hpp"
#include "experimentalTests.hpp"
#include "filesystemTests.hpp"
#include "../utils/logging.hpp"

int main()
  {
	logging::Logger::init();
	UnitTestResult test_result, total_result;

	test_result = hyperbolicTests__all();
	total_result += test_result;
	printTestResult("Hyperbolic Tests", test_result);

	test_result = discreteFuncTests__all();
	total_result += test_result;
	printTestResult("Discrete Function Tests", test_result);

	test_result = formattersTests__all();
	total_result += test_result;
	printTestResult("Formatters Tests", test_result);

	test_result = experimentalTests__all();
	total_result += test_result;
	printTestResult("Experimental Tests", test_result);

	test_result = filesystemTests__all();
	total_result += test_result;
	printTestResult("Filesystem Tests", test_result);

	LOG("--------------------------------");
	printTestResult("All Tests", total_result);
  }
