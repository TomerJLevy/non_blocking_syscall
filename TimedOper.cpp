/*
 * TimedOper.cpp
 *
 *  Created on: Aug 21, 2019
 *      Author: ran-at & Tomer Levy
 */

#include "TimedOper.h"
#include "signal.h"

using namespace AsyncCpp;

TimedOper::SafeSignalsStack::SafeSignalsStack() {
    for (auto iSig = SIGRTMIN; iSig <= SIGRTMAX; ++iSig) {
        signal(iSig, SIG_IGN);
        siginterrupt(iSig, true); //enforce POSIX semantics (NOT retry the SysCall command)
        m_set.insert(iSig);
    }
}

void TimedOper::SafeSignalsStack::push(const SIGNAL& sig) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_set.insert(sig);
}

SIGNAL TimedOper::SafeSignalsStack::pop() {
    std::lock_guard<std::mutex> lock(m_mutex);
    SIGNAL sig = POP_FAILED;
    if (! m_set.empty()) {
       sig = (*m_set.begin());
       m_set.erase(m_set.begin());
    }
    return sig;
}

TimedOper::SignalStackGuard::SignalStackGuard(SafeSignalsStack& lSignalStack)
:   m_SignalStack(lSignalStack)  {
    m_signal = m_SignalStack.pop();
}

TimedOper::SignalStackGuard::~SignalStackGuard() {
    m_SignalStack.push(m_signal);
}

const SIGNAL& TimedOper::SignalStackGuard::get() const {
    return m_signal;
}

TimedOper& TimedOper::instance() {
    static TimedOper myInstance;
    return myInstance;
}

void TimedOper::sig_handler(int signum) {
    signal(signum, SIG_IGN);
}

void TimedOper::Kill(std::thread& thread, SIGNAL signum) {
    pthread_kill(thread.native_handle(), signum);
}
