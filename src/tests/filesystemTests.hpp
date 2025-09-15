#pragma once
#include "unittests.hpp"
#include "isbell.hpp"


inline bool projectAnalysisTest()
{
	try {
		bool passed = true;
		Path p = Path(filesystem::current_path().parent_path().parent_path().parent_path().parent_path() / "src");

		return passed;
	}
	catch (...) {
		THROW(SystemError, "Project analysis test failed");
		return false;
	}
}

inline UnitTestResult filesystemTests__all()
{

	int passed = 0;
	int all = 1;

	if (projectAnalysisTest())
		passed++;

	return UnitTestResult(passed, all);
}
