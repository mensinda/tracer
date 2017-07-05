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
#include "DefaultPrinter.hpp"
#include "Tracer.hpp"
#include <iomanip>
#include <sstream>

#if __cplusplus <= 201402L
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

using namespace tracer;
using namespace std;

DefaultPrinter::DefaultPrinter(Tracer *t) : AbstractPrinter(t) {}
DefaultPrinter::~DefaultPrinter() {}

void DefaultPrinter::calcMaxNameLengths() {
  auto *frames = trace->getFrames();
  for (auto &i : *frames) {
    string module   = i.moduleName;
    string fileName = i.fileName;

    if (cfg.shortenFiles) {
      fs::path modulePath(module);
      fs::path fileNamePath(fileName);

      module   = modulePath.filename();
      fileName = fileNamePath.filename();
    }

    if (module.length() > maxModuleNameLegth)
      maxModuleNameLegth = module.length();

    if (i.funcName.length() > maxFuncNameLegth)
      maxFuncNameLegth = i.funcName.length();

    stringstream address;
    address << hex << i.frameAddr;
    if (address.str().length() > maxAddressLength)
      maxAddressLength = address.str().length();

    string lineInfo = fileName + ":" + to_string(i.line) + ":" + to_string(i.column);
    if (lineInfo.length() > maxLineInfoLength)
      maxLineInfoLength = lineInfo.length();
  }

  calculatedMaxLengths = true;
}


std::string DefaultPrinter::genStringForFrame(size_t frameNum) {
  stringstream outStream;
  auto *       frames = trace->getFrames();

  if (frameNum >= frames->size())
    return "<INVALID PARAMETER>";

  Frame &i = frames->at(frameNum);

  if (!calculatedMaxLengths)
    calcMaxNameLengths();

  string address  = "<N/A>";
  string funcName = "<N/A>";
  string lineInfo = "<N/A>";
  string module   = "<N/A>";


  string moduleP  = i.moduleName;
  string fileName = i.fileName;

  if (cfg.shortenFiles) {
    fs::path modulePath(moduleP);
    fs::path fileNamePath(fileName);

    moduleP  = modulePath.filename();
    fileName = fileNamePath.filename();
  }


  if ((i.flags & FrameFlags::HAS_ADDRESS) == FrameFlags::HAS_ADDRESS) {
    stringstream addressStream;
    addressStream << hex << "0x" << setfill('0') << setw(static_cast<int>(maxAddressLength)) << i.frameAddr;
    address = addressStream.str();
  }

  if ((i.flags & FrameFlags::HAS_FUNC_NAME) == FrameFlags::HAS_FUNC_NAME) {
    funcName = i.funcName;
  }

  if ((i.flags & FrameFlags::HAS_LINE_INFO) == FrameFlags::HAS_LINE_INFO) {
    lineInfo = fileName + ":" + to_string(i.line) + ":" + to_string(i.column);
  }

  if ((i.flags & FrameFlags::HAS_MODULE_INFO) == FrameFlags::HAS_MODULE_INFO) {
    module = moduleP;
  }

  outStream << cfg.prefix << setfill(' ') << left << setw(static_cast<int>(maxFuncNameLegth)) << funcName << cfg.seper1
            << setw(static_cast<int>(maxLineInfoLength)) << lineInfo << cfg.seper2
            << setw(static_cast<int>(maxModuleNameLegth)) << module << cfg.seper3 << address << cfg.suffix << endl;

  return outStream.str();
}
