#ifndef UTILS_H
#define UTILS_H

#include <string>

std::string addExtension(const std::string &fileName, const std::string &extension);

void printTitle();

bool equal(double value1, double value2, double treshold);

void pause(const std::string &msg);

void emptyBuffer();

std::string setRelativePath(const std::string &path, std::string &fileName, const
std::string &extension);
#endif // UTILS_H
