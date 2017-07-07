# tracer [![Build Status](https://travis-ci.org/mensinda/tracer.svg?branch=master)](https://travis-ci.org/mensinda/tracer) [![Build status](https://ci.appveyor.com/api/projects/status/lwnc7tv8qy2af7ck?svg=true)](https://ci.appveyor.com/project/mensinda/tracer)
*nix / Windows Stack trace generator

# Usage

TODO

# Status

|   OS    | Backend            | Type | Status             |
|---------|--------------------|------|--------------------|
| Windows | WIN32              | `TB` | :heavy_check_mark: |
| Linux   | libunwind          | `T-` | :heavy_check_mark: |
|         | glibc bactrace     | `T-` | :heavy_check_mark: |
|         | libelf / elfutils  | `-D` | :heavy_check_mark: |
|         | libbfd / binutils  | `-D` | :heavy_check_mark: |
|         | addr2line fallback | `-D` | :warning:          |

Types:
  - `T-` Stack trace generator
  - `-D` Debug information extractor
  - `TD` Both
  
Status:
  - :heavy_check_mark: works and testet
  - :warning: works but not recommended
  - :heavy_exclamation_mark: broken
