#include <string>
#include "utils.h"

std::string addExtension(const std::string &fileName, const std::string &extension) {
	std::string fn = fileName, ext = extension;

	if (extension.empty())
		ext = ".txt";

	if (fileName.empty())
		return "no_name" + ext;

	int extLen = ext.size();
	std::string searchedExt = fn.substr(fn.size() - extLen, extLen);
	if (searchedExt == ext) {
		return fn;
	}
	return fn + ext;
}

bool equal(double value1, double value2) {
	return value1 < (value2 + 0.00001) && value1 > (value2 - 0.00001);
}
