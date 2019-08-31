/*
 * Cppasio.h
 *
 *  Created on: Aug 30, 2019
 *      Author: fpd
 */

#ifndef CPPASIO_H_
#define CPPASIO_H_

#include <thread>
#include <string>

namespace cppasio {

class AsioOper {
public:
    AsioOper(const AsioOper&) = delete;
    AsioOper(AsioOper&&) = delete;
    AsioOper& operator=(const AsioOper&) = delete;
    AsioOper& operator=(AsioOper&&) = delete;

    static void run(std::function<bool()>&& aFunc, std::function<void()>&& aCallback, std::function<void()>&& aFailFunc);

    static void read(const std::string& aFile, std::function<void(const std::string)>&& aCallback, std::function<void(const std::string&)>&& aFailFunc);

    static void write(const std::string& aFile, const std::string& aContent, std::function<void(const std::string&)>&& aCallback, std::function<void(const std::string&)>&& aFailFunc);

private:
    AsioOper() = default;
    ~AsioOper() = default;

    static void run(std::function<void()>&& aFunc);
    static bool readFile(const std::string& aFile, std::string& aContent);
    static bool writeFile(const std::string& aFile, const std::string& aContent);
};

}


#endif /* CPPASIO_H_ */
