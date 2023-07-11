#include <string>
#include <iostream>
#include <cmath>
#include "utils.h"

std::string addExtension(const std::string &fileName, const std::string &extension) {
    std::string fn = fileName, ext = extension;

    if (extension.empty())
        ext = ".txt";

    if (fileName.empty())
        return "no_name" + ext;

    size_t extLen =  ext.size();
    std::string searchedExt = fn.substr(fn.size() - extLen, extLen);
    if (searchedExt == ext) {
        return fn;
    }
    return fn + ext;
}

void printTitle() {
    std::cout << "______                     _____  _\n"
                 "|  _  \\                   /  __ \\| |                                \n"
                 "| | | | ___   ___  _ __   | /  \\/| |  ___   __ _  _ __    ___  _ __ \n"
                 "| | | |/ _ \\ / _ \\| '_ \\  | |    | | / _ \\ / _` || '_ \\  / _ \\| '__|\n"
                 "| |/ /|  __/|  __/| |_) | | \\__/\\| ||  __/| (_| || | | ||  __/| |   \n"
                 "|___/  \\___| \\___|| .__/   \\____/|_| \\___| \\__,_||_| |_| \\___||_|   \n"
                 "                  | |                                By group 14"
                 "          \n"
                 "                  |_|\n\n";
}

bool equal(double value1, double value2, double treshold) {
    return value1 < (value2 + treshold) && value1 > (value2 - treshold);
}

void pause(const std::string &msg){
    emptyBuffer();
    std::string s;
    std::cout << msg;
    std::getline(std::cin, s);
}

void emptyBuffer(){
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

std::string setRelativePath(const std::string &path, std::string &fileName, const
std::string &extension) {
    std::string ext;
    size_t pointPos;

    // get the extension
    pointPos = fileName.find_last_of('.');
    if (pointPos != std::string::npos)
        ext = fileName.substr(pointPos);

    // If the given file has a wrong extension
    if (ext != extension && pointPos != std::string::npos) {
        std::cout << fileName << " : File extension not supported. please give a "
        << extension << " file.\n";
        fileName.clear();
        pause("Press a key to quit...");
        exit(EXIT_SUCCESS);
    }

    // If the file has no extension
    if (pointPos == std::string::npos)
        fileName += extension;

    return path + fileName;
}