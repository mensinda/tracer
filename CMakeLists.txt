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

cmake_minimum_required( VERSION 3.5 )
project( tracer LANGUAGES CXX )

message( STATUS "Using CMake version: ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}\n" )
include( ${PROJECT_SOURCE_DIR}/cmake/cmakeBuildTools/cmakeBuildTools.cmake )
include( CMakePackageConfigHelpers )

######################
# Set some variables #
######################

# Set the binary output directories
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake )

set( CMAKE_CXX_STANDARD          14 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_C_STANDARD            11 )
set( CMAKE_C_STANDARD_REQUIRED   ON )

set( CMAKE_POSITION_INDEPENDENT_CODE ON )

option( DISABLE_TESTS      "Disables all tests"            OFF )
#option( PREFER_STATIC_LIBS "Prefer linking to static libs" ON  )

################################
# Set up the build environment #
################################

add_compiler(
  GNU MIN_VERSION "7.1"
  ALL        "-Wall -Wextra -Wfloat-equal -Wshadow -Wno-unknown-pragmas -Wno-implicit-fallthrough -fno-strict-aliasing -fdiagnostics-color=always ${GCC_STDLIB_FLAGS}"
  DEBUG      "-Og -Werror -fno-omit-frame-pointer -gcolumn-info" ${COVERAGE_COMPILER_FLAG}
  RELEASE    "-O3 -fno-omit-frame-pointer -g -gcolumn-info"

  C_ALL      "-Wall -Wextra -Wfloat-equal -Wshadow -Wno-unknown-pragmas -std=c11 -fdiagnostics-color=always"
  C_DEBUG    "-Og -Werror -fno-omit-frame-pointer -gcolumn-info" ${COVERAGE_COMPILER_FLAG}
  C_RELEASE  "-O3 -fno-omit-frame-pointer -g -gcolumn-info"

  SANITIZE   "-fsanitize=${SANITIZERS}"
)

add_compiler(
  Clang MIN_VERSION 3.9
  ALL        "-Weverything -ftemplate-backtrace-limit=0 -fno-strict-aliasing -fcolor-diagnostics"
             "-Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded ${CLANG_STDLIB_FLAGS}"
             "-Wno-gnu-anonymous-struct -Wno-nested-anon-types -Wno-exit-time-destructors"
             "-Wno-global-constructors -Wno-switch-enum -Wno-reserved-id-macro"
             "-Wno-documentation-unknown-command -Wno-unknown-pragmas"
             "-Wno-disabled-macro-expansion -save-temps"
  DEBUG      "-Werror -gline-tables-only"
  RELEASE    "-O3 -gline-tables-only"

  C_ALL      "-Weverything -ftemplate-backtrace-limit=0  -std=c11 -fcolor-diagnostics"
             "-Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-float-equal"
             "-Wno-gnu-anonymous-struct -Wno-nested-anon-types -Wno-exit-time-destructors"
             "-Wno-global-constructors -Wno-switch-enum -Wno-reserved-id-macro"
             "-Wno-documentation-unknown-command -Wno-unknown-pragmas"
             "-Wno-disabled-macro-expansion"
  C_DEBUG    "-Werror -gline-tables-only"
  C_RELEASE  "-O3 -gline-tables-only"

  SANITIZE "-fsanitize=${SANITIZERS}"
)

######################
# Check Dependencies #
######################

set( DEPS stdc++fs )

if( UNIX )
  find_package( LibUnwind 1.2.1 REQUIRED )
  find_package( LibELF          REQUIRED )
  set( DEPS ${DEPS} LibUnwind::LibUnwind LibELF::DW )
endif( UNIX )

run_git()

############################
# Setting build conditions #
############################

add_platform( OS WIN32 TARGET windows )

add_platform(
  OS UNIX
  TARGET
    libunwind glibc # Tracer
    libdwlf         # Debug Info
)


# Automatically add a debug info platform target if non is set
foreach( I IN ITEMS "UNIX_LIBUNWIND" "UNIX_GLIBC" )
  if( "${I}" IN_LIST PLATFORM_TARGET )
    set( FOUND OFF )
    foreach( J IN ITEMS "UNIX_LIBDWLF" )
      if( "${J}" IN_LIST PLATFORM_TARGET )
        set( FOUND ON )
        break()
      endif()
    endforeach()

    if( NOT FOUND )
      list( APPEND PLATFORM_TARGET "UNIX_LIBDWLF" )
    endif()

    break()
  endif()
endforeach()

check_platform()

####################
# Generate subdirs #
####################

new_project_library(
  PATH         "${PROJECT_SOURCE_DIR}/src"
  NAME         "tracer"
  TEMPLATE     "${PROJECT_SOURCE_DIR}/cmake/templates/CMakeLists.lib.txt"
  DEPENDENCIES ${DEPS}
)

if( NOT DISABLE_TESTS )
  file( GLOB  TEST_DIRS "${PROJECT_SOURCE_DIR}/tests/*/" )
  enable_testing()

  add_custom_target(
    check
      COMMAND ${CMAKE_COMMAND} -E env CTEST_OUTPUT_ON_FAILURE=1 ${CMAKE_CTEST_COMMAND} --force-new-ctest-process
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )

  foreach( I IN LISTS TEST_DIRS )
    file( RELATIVE_PATH J "${PROJECT_SOURCE_DIR}/tests" "${I}" )

    new_project_executable(
      PATH         "${I}"
      NAME         "${J}"
      TEMPLATE     "${PROJECT_SOURCE_DIR}/cmake/templates/CMakeLists.exe.txt"
      DEPENDENCIES ${DEPS} tracer
    )

    add_dependencies( check ${J} )
  endforeach( I IN LISTS TEST_DIRS )
endif( NOT DISABLE_TESTS )

##############################
# Update include directories #
##############################

include_directories( SYSTEM
  ${LibUnwind_INCLUDE_DIRS}
)

include_directories(
  ${tracer_LIB_INCLUDE_DIRECTORIES}
  ${PROJECT_BINARY_DIR}
)

######################
# Add subdirectories #
######################

foreach( I IN LISTS tracer_SUBDIR_LIST )
  add_subdirectory( "${I}" )
endforeach( I IN LISTS tracer_SUBDIR_LIST )

##################
# Generate Files #
##################

configure_file( "${PROJECT_SOURCE_DIR}/cmake/templates/defines.in.hpp" "${PROJECT_BINARY_DIR}/defines.hpp" @ONLY )

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Version.cmake"
    VERSION       ${CM_VERSION_MAJOR}.${CM_VERSION_MINOR}.${CM_VERSION_PATCH}
    COMPATIBILITY SameMajorVersion
)

install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Version.cmake
    ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}Config.cmake
  DESTINATION lib/cmake/${PROJECT_NAME}
)

install( EXPORT ${PROJECT_NAME}Targets             DESTINATION lib/cmake/${PROJECT_NAME} )
install( FILES "${PROJECT_BINARY_DIR}/defines.hpp" DESTINATION include/tracer )
