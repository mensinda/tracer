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

class DefaultPrinter : public AbstractPrinter {
 public:
  struct Config {
    std::string prefix = " in ";
    std::string seper1 = " at ";
    std::string seper2 = " -- ";
    std::string seper3 = " [";
    std::string suffix = "]";

    bool shortenFiles = true;
  };

 private:
  unsigned int maxModuleNameLegth = 5;
  unsigned int maxLineInfoLength  = 5;
  unsigned int maxAddressLength   = 5;
  unsigned int maxFuncNameLegth   = 5;

  bool calculatedMaxLengths = false;

  Config cfg;

  void calcMaxNameLengths();

 public:
  DefaultPrinter() = delete;
  DefaultPrinter(Tracer *t);
  virtual ~DefaultPrinter();

  void setConfig(Config newCfg) { cfg = newCfg; }

  Config getConfig() const { return cfg; }

  std::string genStringForFrame(size_t frameNum) override;
};
}
