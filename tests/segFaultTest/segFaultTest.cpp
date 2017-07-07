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
#include "FancyPrinter.hpp"
#include "Tracer.hpp"
#include <iostream>
#include <signal.h>

#ifndef _WIN32
#include <unistd.h>
#endif
#ifdef __GNUC__
#define noinline __attribute__ ((noinline))
#else
#define noinline
#endif

using namespace std;
using namespace tracer;

static void handler(int signum);
static void handler(int signum) {
  Tracer t1;
  t1();
  FancyPrinter p1(&t1);
  p1.setSignum(signum);
  p1.printToStdErr();

#if defined(__GNUC__) || defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
#endif
  signal(signum, SIG_DFL);
#if defined(__GNUC__) || defined(__clang__)
#pragma clang diagnostic pop
#endif

#ifndef _WIN32
  kill(getpid(), signum);
#endif
}

int f1(int x);
int f2(int x);
int f3(int x);
int f4(int x);
int f5(int x);

int noinline f1(int x) { return f2(++x); }
int noinline f2(int x) { return f3(++x); }
int noinline f3(int x) { return f4(++x); }
int noinline f4(int x) { return f5(++x); }
int noinline f5(int x) {
  int *volatile p = nullptr;
  *p     = x; // SEGFAULT here
  return i;
}

int main() {

  signal(SIGSEGV, handler);
  f1(42);

  return 0;
}
