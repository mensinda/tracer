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
#include "DebugInfoBFD.hpp"
#include "whereami.h"
#include <cxxabi.h>
#include <dlfcn.h>
#include <iostream>
#include <unistd.h>

using namespace tracer;
using namespace tracer::internal;
using namespace std;

DebugInfoBFD::DebugInfoBFD() {}
DebugInfoBFD::~DebugInfoBFD() {}

bool DebugInfoBFD::isBfdInit = false;


void DebugInfoBFD::findInSection(bfd *abfd, bfd_section *section, void *ctx) {
  DebugInfoBFD *obj = reinterpret_cast<DebugInfoBFD *>(ctx);

  if (obj->ctx.found)
    return; // Nothing left to do

  if ((bfd_get_section_flags(abfd, section) & SEC_ALLOC) == 0)
    return;

  bfd_vma       vma  = bfd_get_section_vma(abfd, section);
  bfd_size_type size = bfd_get_section_size(section);

  //   cout << "aaa " << vma << ":" << size << " " << obj->ctx.addr << endl;

  // Check boundaries
  if (obj->ctx.addr < vma || obj->ctx.addr >= vma + size) {
    auto tempAddr = obj->ctx.addr - obj->ctx.baseAddr; // Try again with adjusted values
    if (tempAddr < vma || tempAddr >= vma + size) {
      return;
    }
  }

  //   cout << "bbb" << endl;

  const char * fileName_CSTR = nullptr;
  const char * funcName_CSTR = nullptr;
  unsigned int lineNumber    = 0;
  unsigned int column        = 0;


  if (bfd_find_nearest_line_discriminator(abfd,
                                          section,
                                          obj->ctx.secSym->symbols,
                                          obj->ctx.addr - vma,
                                          &fileName_CSTR,
                                          &funcName_CSTR,
                                          &lineNumber,
                                          &column) == 0) {
    if (bfd_find_nearest_line_discriminator(abfd,
                                            section,
                                            obj->ctx.dynSecSym->symbols,
                                            obj->ctx.addr - vma,
                                            &fileName_CSTR,
                                            &funcName_CSTR,
                                            &lineNumber,
                                            &column) != 0) {
      return;
    }
  }

  obj->ctx.found = true;

  obj->ctx.line = static_cast<int>(lineNumber);
  obj->ctx.col  = static_cast<int>(column);

  if (fileName_CSTR)
    obj->ctx.fileName = fileName_CSTR;

  if (funcName_CSTR) {
    char   demangled[constants::MAX_FUNC_NAME];
    int    status = 0;
    size_t legth  = sizeof(demangled);
    abi::__cxa_demangle(funcName_CSTR, demangled, &legth, &status);

    if (status == 0) {
      obj->ctx.funcName = demangled;
    } else {
      obj->ctx.funcName = funcName_CSTR;
    }
  }
}



bool DebugInfoBFD::processFrames(std::vector<Frame> &frames) {
  if (!isBfdInit) {
    bfd_init();
    isBfdInit = true;
  }

  int exePathLength = wai_getExecutablePath(nullptr, 0, nullptr);
  if (exePathLength < 0) {
    cerr << "[TRACER] (libbfd) unable to determine the current executable path" << endl;
    return false;
  }

  char *exePath = reinterpret_cast<char *>(malloc(static_cast<size_t>(exePathLength + 1)));
  if (!exePath) {
    cerr << "[TRACER] (libbfd) malloc failed....." << endl;
    return false;
  }

  wai_getExecutablePath(exePath, exePathLength, nullptr);

  for (auto &i : frames) {
    Dl_info dlInfo;

    if (dladdr(reinterpret_cast<void *>(i.frameAddr), &dlInfo) == 0) {
      continue;
    }

    if (dlInfo.dli_sname) {
      char   demangled[constants::MAX_FUNC_NAME];
      int    status = 0;
      size_t legth  = sizeof(demangled);
      abi::__cxa_demangle(dlInfo.dli_sname, demangled, &legth, &status);

      i.flags |= FrameFlags::HAS_FUNC_NAME;

      if (status == 0) {
        i.funcName = demangled;
      } else {
        i.funcName = dlInfo.dli_sname;
      }
    }

    if (dlInfo.dli_fname) {
      i.flags |= FrameFlags::HAS_MODULE_INFO;
      i.moduleName = dlInfo.dli_fname;
    }

    // Start getting line info
    abfdRAII    abfd;
    asymbolRAII symbols;
    asymbolRAII dynSymbols;

    abfd.abfd = bfd_openr(exePath, nullptr);
    if (abfd() == nullptr)
      continue;

    abfd()->flags |= BFD_DECOMPRESS;

    if (bfd_check_format(abfd(), bfd_object) != TRUE)
      continue;

    if ((bfd_get_file_flags(abfd()) & HAS_SYMS) == 0)
      continue; // No debug symbols?

    auto storageSize        = bfd_get_symtab_upper_bound(abfd());
    auto dynamicStorageSize = bfd_get_dynamic_symtab_upper_bound(abfd());

    if (storageSize <= 0 && dynamicStorageSize <= 0)
      continue;

    if (storageSize > 0) {
      symbols.symbols = static_cast<bfd_symbol **>(malloc(static_cast<size_t>(storageSize)));
      bfd_canonicalize_symtab(abfd(), symbols());
    }

    if (dynamicStorageSize > 0) {
      dynSymbols.symbols = static_cast<bfd_symbol **>(malloc(static_cast<size_t>(dynamicStorageSize)));
      bfd_canonicalize_dynamic_symtab(abfd(), dynSymbols());
    }

    ctx = FindInSectionContext();

    ctx.addr      = i.frameAddr - 4;
    ctx.baseAddr  = reinterpret_cast<Address>(dlInfo.dli_fbase);
    ctx.secSym    = &symbols;
    ctx.dynSecSym = &dynSymbols;

    bfd_map_over_sections(abfd(), &findInSection, reinterpret_cast<void *>(this));

    if (ctx.found) {
      if (!ctx.fileName.empty()) {
        i.flags |= FrameFlags::HAS_LINE_INFO;
        i.fileName = ctx.fileName;
        i.line     = ctx.line;
        i.column   = ctx.col;
      }

      if (!ctx.funcName.empty()) {
        i.flags |= FrameFlags::HAS_FUNC_NAME;
        i.funcName = ctx.funcName;
      }
    }
  }

  free(exePath);

  return true;
}