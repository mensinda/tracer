/* Copyright (c) 2017, Daniel Mensinger
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Daniel Mensinger nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Daniel Mensinger BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "defines.hpp"
#include "LibUnwindTracer.hpp"
#include <cxxabi.h>
#include <iostream>

using namespace tracer;

LibUnwindTracer::LibUnwindTracer() {}

bool LibUnwindTracer::init() {
  if (unw_getcontext(&context) != 0) {
    std::cerr << "[TRACER] (libunwind) Failed to get context" << std::endl;
    return false;
  }

  if (unw_init_local(&cursor, &context) != 0) {
    std::cerr << "[TRACER] (libunwind) Failed to initialize" << std::endl;
    return false;
  }

  while (unw_step(&cursor) > 0) {
    unw_word_t ip;
    unw_word_t offset;
    unw_get_reg(&cursor, UNW_REG_IP, &ip);

    char funcName[constants::MAX_FUNC_NAME];
    unw_get_proc_name(&cursor, funcName, sizeof(funcName), &offset);

    char   demangled[constants::MAX_FUNC_NAME];
    int    status = 0;
    size_t legth  = sizeof(demangled);
    abi::__cxa_demangle(funcName, demangled, &legth, &status);

    std::string funcNameStr;
    if (status == 0) {
      funcNameStr = demangled;
    } else {
      funcNameStr = funcName;
    }

    std::cout << "[TRACER] ADDR: " << ip << " -- " << funcNameStr << std::endl;
  }

  return true;
}

void LibUnwindTracer::print() { std::cout << "BACKTRACE" << std::endl; }
