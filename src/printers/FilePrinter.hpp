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
#include "DefaultPrinter.hpp"
#include <vector>

#if !DISABLE_STD_FILESYSTEM
#if __cplusplus <= 201402L
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif
#endif

namespace tracer {

/*!
 * \brief Prints file contents when line information is avaliable in the frame
 */
class FilePrinter : virtual public DefaultPrinter {
 public:
  struct FileConfig {
    unsigned int maxRecursionDepth = 4;
    unsigned int linesBefore       = 4;
    unsigned int linesAfter        = 4;

    std::string lineHighlightColor = "\x1b[1;31m";
  };

 private:
  FileConfig fCFG;

#if !DISABLE_STD_FILESYSTEM
  std::vector<fs::path> pathCache;
  bool findPath(unsigned int depth, fs::path current, fs::path &out, fs::path const &file);
#endif

 public:
  FilePrinter() = delete;
  FilePrinter(Tracer *t);
  virtual ~FilePrinter();

  std::string genStringPostFrame(size_t frameNum) override;

#if !DISABLE_STD_FILESYSTEM
  fs::path findFile(std::string file);
#endif

  void setFilePrinterConfig(FileConfig d) { fCFG = d; }

  FileConfig getFilePrinterConfig() { return fCFG; }
};
}
