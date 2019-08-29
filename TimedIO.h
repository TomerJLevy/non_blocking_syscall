/*
 * TimedIO.h
 *
 *  Created on: Aug 21, 2019
 *      Author: Ran Atias & Tomer Levy
 */

#ifndef TIMED_IO_H_
#define TIMED_IO_H_

#include <iostream>
#include <future>
#include <signal.h>
#include <string.h>
#include <set>

// This class allows to set timeout for IO calls.

namespace AsyncCpp {
using SIGNAL = int;

class  TimedIO {
	class SignalStackGuard;

	class SafeSignalsStack {
		friend class SignalStackGuard;

	public:
		SafeSignalsStack();

		static constexpr SIGNAL POP_FAILED = -1;
	private:
		void push(const SIGNAL& sig);
		SIGNAL pop();

		std::mutex m_mutex;
		std::set<SIGNAL> m_set; // I have used std::set for as a unique_stack
	};

	class SignalStackGuard {
	public:
		SignalStackGuard(SafeSignalsStack& lSignalStack);
		~SignalStackGuard();

		const SIGNAL& get() const;
	private:
		SIGNAL m_signal;
		SafeSignalsStack& m_SignalStack;
	};

public:

static TimedIO& instance();

// delete copy and move constructors and assign operators
TimedIO(TimedIO const&) = delete;             // Copy construct
TimedIO(TimedIO&&) = delete;                  // Move construct
TimedIO& operator=(TimedIO const&) = delete;  // Copy assign
TimedIO& operator=(TimedIO &&) = delete;      // Move assign


template<typename FUNCTION>
static void promSetter(FUNCTION&& aFunc, std::shared_ptr<std::promise<void>> aProm) {
    aFunc();
    aProm->set_value();
}
template<typename FUNCTION, typename T>
static void promSetter(FUNCTION&& aFunc, std::shared_ptr<std::promise<T>> aProm) {
    auto res = aFunc();
    aProm->set_value(res);
}

template<typename FUNCTION>
auto try_run_for(FUNCTION&& aFunc, int aTimeout) -> std::future<decltype(aFunc())> {
    SignalStackGuard lSignalStackGuard(m_signalsStack);
    auto& lSignal = lSignalStackGuard.get();
    if (lSignal == SafeSignalsStack::POP_FAILED) {
        throw std::runtime_error("Failed to start the operation");
    }

    std::thread lThread;
    auto fut = asyncJob(aFunc, lThread, lSignal);

    std::future_status status = fut.wait_for(std::chrono::seconds(aTimeout));
    if (status != std::future_status::ready) {
        std::cout << "KILL" << std::endl;
        Kill(lThread, lSignal);
        lThread.join();
        throw std::runtime_error("timeout passed!");
    }
    lThread.join();
    return fut;
}

private:
TimedIO() = default;
~TimedIO() = default;

template<typename FUNCTION>
static auto asyncJob(FUNCTION&& aFunc, std::thread& aThread, SIGNAL signum) -> std::future<decltype(aFunc())> {
	using ret_type = decltype(aFunc());
	auto prom = std::make_shared<std::promise<ret_type>>();
	aThread = std::thread([=]{
		try {
			signal(signum, sig_handler);
			promSetter(aFunc, prom);
		} catch (...) {
			prom->set_exception(std::current_exception());
		}
		signal(signum, SIG_IGN);
		});
	return prom->get_future();
}

static void sig_handler(int signum);
static void Kill(std::thread& thread, SIGNAL signum);

SafeSignalsStack m_signalsStack;
};
}

#endif /* TIMED_IO_H_ */
