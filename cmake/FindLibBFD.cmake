# Copyright (c) 2017, Daniel Mensinger
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the Daniel Mensinger nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL Daniel Mensinger LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

include(FindPackageHandleStandardArgs)

find_package( DLFCN )

find_path(
  LibBFD_INC
    NAMES bfd.h
    HINTS ${LibBFD_ROOT}/include
)

find_library(
  LibBFD_DW_LIB
    NAMES bfd
    HINTS
      /usr/lib
      ${LibBFD_ROOT}/lib
)

find_package_handle_standard_args(
  LibBFD
  REQUIRED_VARS LibBFD_INC LibBFD_DW_LIB DLFCN_FOUND
)

if( LibBFD_FOUND )
  set( LibBFD_INCLUDE_DIRS ${LibBFD_INC} )
  set( LibBFD_LIBRARIES    ${LibBFD_DW_LIB})

  if( NOT TARGET LibBFD::LibBFD )
    add_library(LibBFD::LibBFD UNKNOWN IMPORTED)
    set_target_properties( LibBFD::LibBFD
      PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${LibBFD_INCLUDE_DIRS}"
        IMPORTED_LOCATION             "${LibBFD_LIBRARIES}"
    )
  endif( NOT TARGET LibBFD::LibBFD )
endif()
