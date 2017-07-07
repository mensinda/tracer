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

#pragma once

#include "defines.hpp"
#include "AbstractPrinter.hpp"

namespace tracer {

class DefaultPrinter : virtual public AbstractPrinter {
 public:
  struct Config {
    std::string prefix = " \x1b[0;33min ";
    std::string seper1 = " \x1b[0;33mat ";
    std::string seper2 = " \x1b[0;33m-- ";
    std::string seper3 = " \x1b[0;33m[";
    std::string suffix = "\x1b[0;33m]\x1b[0m";

    std::string colorFrameNum = "\x1b[1;36m";
    std::string colorNotFound = "\x1b[1;33m";
    std::string colorAddress  = "\x1b[0;36m";
    std::string colorFuncName = "\x1b[1;31m";
    std::string colorLineInfo = "\x1b[1;32m";
    std::string colorModule   = "\x1b[1;35m";

    bool shortenFiles      = false; // The source file path
    bool shortenModules    = true;  // The executable module (.so/.dll/.exe)
    bool canonicalizePaths = true;
  };

 private:
  size_t maxModuleNameLegth = 5;
  size_t maxLineInfoLength  = 5;
  size_t maxAddressLength   = 5;
  size_t maxFuncNameLegth   = 5;

  bool calculatedMaxLengths = false;

  Config cfg;

  void calcMaxNameLengths();

 protected:
  std::string genStringForFrameIMPL(size_t frameNum) override;
  void setupTrace() override;

 public:
  DefaultPrinter();
  DefaultPrinter(Tracer *t);
  virtual ~DefaultPrinter();

  void setConfig(Config newCfg) { cfg = newCfg; }

  Config getConfig() const { return cfg; }
};
}
