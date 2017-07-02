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
#include "Tracer.hpp"
#include <iostream>

#if USE_LIBUNWIND
#include "LibUnwindTracer.hpp"
#endif

#if USE_GLIBC
#include "GlibCTracer.hpp"
#endif

#if USE_DWFL
#include "DebugInfoDWLF.hpp"
#endif

using namespace tracer;

Tracer::Tracer() : Tracer(getAvaliableEngines()[0], getAvaliableDebuggers()[0]) {}

Tracer::Tracer(TraceerEngines engine, DebuggerEngines debugger) {
#if USE_LIBUNWIND
  if (engine == TraceerEngines::LIBUNWIND)
    tracerEngine = new LibUnwindTracer;
#endif

#if USE_GLIBC
  if (engine == TraceerEngines::GLIBC)
    tracerEngine = new GlibCTracer;
#endif

#if USE_DWFL
  if (debugger == DebuggerEngines::LIBDWFL)
    debuggerEngine = new DebugInfoDWFL;
#endif

  if (!tracerEngine) {
    std::cerr << "[TRACER] Unable to initialize tracer engine" << std::endl;
    return;
  }

  if (!debuggerEngine) {
    std::cerr << "[TRACER] Unable to initialize debugger engine" << std::endl;
    return;
  }
}

Tracer::~Tracer() {
  if (tracerEngine)
    delete tracerEngine;

  if (debuggerEngine)
    delete debuggerEngine;
}

std::vector<Frame> *Tracer::trace() {
  if (!tracerEngine || !debuggerEngine) {
    std::cerr << "[TRACER] Can not generate a backtrace without a tracerEngine or debuggerEngine" << std::endl;
    return nullptr;
  }

  frames = tracerEngine->backtrace();
  debuggerEngine->processFrames(frames);

  return &frames;
}

std::vector<Frame> *Tracer::getFrames() { return &frames; }


void Tracer::print() {
  if (!tracerEngine)
    return;

  //   tracerEngine->print();
}

std::vector<TraceerEngines> Tracer::getAvaliableEngines() {
  std::vector<TraceerEngines> engines;

#if USE_LIBUNWIND
  engines.emplace_back(TraceerEngines::LIBUNWIND);
#endif
#if USE_GLIBC
  engines.emplace_back(TraceerEngines::GLIBC);
#endif
#if USE_WINDOWS
  engines.emplace_back(TraceerEngines::WIN32);
#endif

  return engines;
}

std::vector<DebuggerEngines> Tracer::getAvaliableDebuggers() {
  std::vector<DebuggerEngines> engines;

#if USE_DWFL
  engines.emplace_back(DebuggerEngines::LIBDWFL);
#endif

  return engines;
}
