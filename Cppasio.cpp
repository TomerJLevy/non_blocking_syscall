/*
 * Cppasio.cpp
 *
 *  Created on: Aug 31, 2019
 *      Author: fpd
 */

#include "Cppasio.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace cppasio;

void AsioOper::run(std::function<bool()>&& aFunc, std::function<void()>&& aCallback, std::function<void()>&& aFailFunc) {
    run([&]{
        try {
            if (aFunc()) {
                aCallback();
            } else {
                aFailFunc();
            }
        } catch(...) {
            aFailFunc();
        }
    });
}

void AsioOper::read(const std::string& aFile, std::function<void(const std::string)>&& aCallback, std::function<void(const std::string&)>&& aFailFunc) {
    run([&]{
        try {
            std::string lContent;
            if (readFile(aFile, lContent)) {
                aCallback(lContent);
            } else {
                aFailFunc("Failed to open file" + aFile);
            }
        } catch(...) {
            aFailFunc("Got exception for file" + aFile);
        }
    });
}

void AsioOper::write(const std::string& aFile, const std::string& aContent, std::function<void(const std::string&)>&& aCallback, std::function<void(const std::string&)>&& aFailFunc) {
    run([&]{
        try {
            if (writeFile(aFile, aContent)) {
                aCallback(aFile);
            } else {
                aFailFunc(aFile);
            }
        } catch(...) {
            aFailFunc(aFile);
        }
    });
}

void AsioOper::run(std::function<void()>&& aFunc) {
    std::thread(aFunc).detach();
}

bool AsioOper::writeFile(const std::string& aFile, const std::string& aContent) {
    std::ofstream lFile(aFile);
    if (!lFile.is_open()) {
        return false;
    }

    lFile << aContent;
    lFile.close();
    return true;
}

bool AsioOper::readFile(const std::string& aFile, std::string& aContent) {
    aContent = "";
    std::ifstream lFile(aFile);
    if (!lFile.is_open()) {
        return false;
    }

    std::string lLine;
    while (getline(lFile,lLine) ) {
        aContent += lLine + '\n';
    }
    lFile.close();
    return true;
}
