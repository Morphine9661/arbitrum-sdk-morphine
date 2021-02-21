/*
 * Copyright 2020, Offchain Labs, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef machinethread_hpp
#define machinethread_hpp

#include <avm/machine.hpp>
#include <avm_values/value.hpp>

#include <rocksdb/slice.h>
#include <atomic>
#include <chrono>
#include <memory>
#include <thread>
#include <vector>

class MachineThread : public Machine {
   public:
    typedef enum {
        MACHINE_NONE,     // Not running, no result stored
        MACHINE_RUNNING,  // Thread currently actively running machine
        MACHINE_ABORTED,  // Thread was aborted
        MACHINE_SUCCESS,  // Run finished, results available
        MACHINE_ERROR     // Error occurred
    } machine_status_enum;

   private:
    std::unique_ptr<std::thread> machine_thread;

    // Machine thread communication
    std::atomic<bool> machine_abort{false};
    std::atomic<machine_status_enum> machine_status{MACHINE_NONE};
    std::string machine_error_string;
    Assertion last_assertion;

   public:
    MachineThread() = default;
    ~MachineThread() { abortMachine(); }
    explicit MachineThread(MachineState machine_state_)
        : Machine(std::move(machine_state_)) {}
    MachineThread(std::shared_ptr<Code> code, value static_val)
        : Machine(std::move(code), std::move(static_val)) {}

    bool runMachine(MachineExecutionConfig config);
    void abortMachine();
    machine_status_enum status();
    std::string getErrorString();
    void clearError();
    Assertion nextAssertion();
    void operator()(MachineExecutionConfig config);
};

#endif /* machine_hpp */
