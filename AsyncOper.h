/*
 * AsyncOper.h
 *
 *  Created on: Aug 30, 2019
 *      Author: Tomer Levy
 */

#ifndef _ASYNC_OPER_H_
#define _ASYNC_OPER_H_

#include <thread>
#include <string>

namespace AsyncCpp {

class AsyncOper {
public:
    AsyncOper(const AsyncOper&) = delete;
    AsyncOper(AsyncOper&&) = delete;
    AsyncOper& operator=(const AsyncOper&) = delete;
    AsyncOper& operator=(AsyncOper&&) = delete;

    static void run(std::function<bool()>&& aFunc, std::function<void()>&& aCallback, std::function<void()>&& aFailFunc);

    static void read(const std::string& aFile, std::function<void(const std::string)>&& aCallback, std::function<void(const std::string&)>&& aFailFunc);

    static void write(const std::string& aFile, const std::string& aContent, std::function<void(const std::string&)>&& aCallback, std::function<void(const std::string&)>&& aFailFunc);

private:
    AsyncOper() = delete;
    ~AsyncOper() = default;

    static void run(std::function<void()>&& aFunc);
    static bool readFile(const std::string& aFile, std::string& aContent);
    static bool writeFile(const std::string& aFile, const std::string& aContent);
};

}

#endif /* _ASYNC_OPER_H_ */
