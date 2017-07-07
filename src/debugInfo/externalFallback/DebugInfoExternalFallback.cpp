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
#include "DebugInfoExternalFallback.hpp"
#include <dlfcn.h>
#include <iostream>
#include <regex>
#include <sstream>
#include <stdio.h>

using namespace tracer;
using namespace std;

DebugInfoExternalFallback::~DebugInfoExternalFallback() {}

const size_t EXEC_TRIES = 4;


bool DebugInfoExternalFallback::processFrames(vector<Frame> &frames) {
  regex  regFunc("^([^\n]*)", regex_constants::ECMAScript);
  regex  regLine("([^\n:]+):([0-9?]+)(:([0-9?]+))?", regex_constants::ECMAScript);
  smatch match;
  string temp;

  for (auto &i : frames) {
    cout << "==================================================" << endl;
    Dl_info dlInfo;
    Address addr1 = i.frameAddr - 4;

    if (dladdr(reinterpret_cast<void *>(addr1), &dlInfo) == 0) {
      continue;
    }

    if (dlInfo.dli_fname) {
      i.flags |= FrameFlags::HAS_MODULE_INFO;
      i.moduleName = dlInfo.dli_fname;
    } else {
      continue; // dlInfo.dli_fname is required for the next step
    }

    cout << "dladdr passed: " << dlInfo.dli_fname << endl;

    Address addr2 = addr1 - reinterpret_cast<Address>(dlInfo.dli_fbase);

    struct _PopenContent {
      stringstream stream;
      string       output;
    } contents[EXEC_TRIES];

    char buff[1024];

    contents[0].stream << "eu-addr2line -fC -e " << dlInfo.dli_fname << " 0x" << hex << addr1 << " 2>&1" << endl;
    contents[1].stream << "eu-addr2line -fC -e " << dlInfo.dli_fname << " 0x" << hex << addr2 << " 2>&1" << endl;
    contents[2].stream << "addr2line -fC -e " << dlInfo.dli_fname << " 0x" << hex << addr1 << " 2>&1" << endl;
    contents[3].stream << "addr2line -fC -e " << dlInfo.dli_fname << " 0x" << hex << addr2 << " 2>&1" << endl;

    for (auto &j : contents) {
      cout << "--------------------------------------------------" << endl;
      cout << "NEW PASS: " << dlInfo.dli_fname << endl;
      FILE *fd = popen(j.stream.str().c_str(), "r");

      if (fd) {
        while (fgets(buff, sizeof(buff), fd)) {
          j.output += buff;
        }

        if (pclose(fd) != 0) {
          cerr << "INVALID RETURN VALUE" << endl;
          continue;
        }
      } else {
        cerr << "NO FD" << endl;
        continue;
      }

      cout << "---------" << endl << j.output << endl << "---------" << endl;

      if (regex_search(j.output, match, regFunc)) {
        cout << "FNAME MATCH" << endl;
        temp = regex_replace(match.str(), regFunc, "$1");

        if (!temp.empty() && temp != "??") {
          i.flags |= FrameFlags::HAS_FUNC_NAME;
          i.funcName = temp;
          cout << "Valid func name: " << temp << endl;
        }
      }

      if (regex_search(j.output, match, regLine)) {
        cout << "LINE MATCH " << match.size() << endl;
        if (match.size() >= 5) {
          string file = match[1];
          string line = match[2];
          string colm = match[4];

          cout << "LN: '" << file << "' '" << line << "' '" << colm << "'" << endl;

          if (!file.empty() && file != "??") {
            i.flags |= FrameFlags::HAS_LINE_INFO;
            i.fileName = file;

            try {
              if (!line.empty())
                i.line = stoi(line);

              if (!colm.empty())
                i.column = stoi(colm);
            } catch (...) {}
            break;
          }
        }
      }
    }
  }
  return true;
}
