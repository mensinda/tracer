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
#include "AbstractPrinter.hpp"
#include "Tracer.hpp"
#include <fstream>
#include <iostream>
#include <regex>

using namespace tracer;
using namespace std;

AbstractPrinter::AbstractPrinter() {}
AbstractPrinter::AbstractPrinter(Tracer *t) : trace(t) {}
AbstractPrinter::~AbstractPrinter() {}

void AbstractPrinter::setupTrace() {}


std::string tracer::AbstractPrinter::generateString() {
  if (!trace)
    return "";

  std::string outSTR;
  auto *      frames = trace->getFrames();

  for (size_t i = 0; i < frames->size(); ++i) {
    outSTR += genStringPreFrameIMPL(i);
    outSTR += genStringForFrameIMPL(i);
    outSTR += genStringPostFrameIMPL(i);
  }

  if (disableColorB) {
    regex escRemover("\x1b\\[[0-9;]*m");
    outSTR = regex_replace(outSTR, escRemover, "");
  }

  return outSTR;
}

string AbstractPrinter::genStringPreFrameIMPL(size_t) { return ""; }
string AbstractPrinter::genStringPostFrameIMPL(size_t) { return ""; }

std::string tracer::AbstractPrinter::genStringPreFrame(size_t frameNum) {
  if (!trace)
    return "";

  return genStringPreFrameIMPL(frameNum);
}

std::string tracer::AbstractPrinter::genStringForFrame(size_t frameNum) {
  if (!trace)
    return "";

  return genStringForFrameIMPL(frameNum);
}

std::string tracer::AbstractPrinter::genStringPostFrame(size_t frameNum) {
  if (!trace)
    return "";

  return genStringPostFrameIMPL(frameNum);
}



void tracer::AbstractPrinter::printToFile(std::string file, bool append) {
  if (!trace)
    return;

  auto mode = ios_base::app | ios_base::out;

  if (!append)
    mode |= ios_base::trunc;

  ofstream outStream(file, mode);

  if (!outStream.is_open())
    return;

  outStream << generateString() << endl;
  outStream.close();
}

void tracer::AbstractPrinter::printToStdErr() { cerr << generateString() << endl; }
void tracer::AbstractPrinter::printToStdOut() { cout << generateString() << endl; }
void tracer::AbstractPrinter::setTrace(tracer::Tracer *t) {
  trace = t;
  setupTrace();
}
