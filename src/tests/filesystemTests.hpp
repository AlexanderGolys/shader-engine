#pragma once
#include "unittests.hpp"
#include "codeAnalysis.hpp"


inline bool projectAnalysisTest()
{
	try {
		bool passed = true;
		Path p = Path(filesystem::current_path().parent_path().parent_path().parent_path().parent_path() / "src");
		CppProject project = CppProject(p);
		auto files = project.listAllCppFiles({});
		int headers = 0;

		for (const auto& file : files)
			if (file.header())
				headers++;

		LOG(format("{} total source files", files.size()));
		LOG(format("{} header files, {} cpp files", headers, files.size() - headers));

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
