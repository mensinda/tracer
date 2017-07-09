# Tracer                                {#mainpage}
*nix / Windows Stack trace generator

## Build Status

| OS      | Status |
|---------|--------|
| Linux   | [![Build Status](https://travis-ci.org/mensinda/tracer.svg?branch=master)](https://travis-ci.org/mensinda/tracer) |
| Windows | [![Build status](https://ci.appveyor.com/api/projects/status/lwnc7tv8qy2af7ck?svg=true)](https://ci.appveyor.com/project/mensinda/tracer) |

# Usage

## Simple setup

This will print a stack trace using the default Printer (FancyPrinter) and the default
Config (TracerHandler::Config)

```cpp
TracerHandler::getTracer()->defaultSetup();
```

## More advanded setup

With this setup it is possible to customize the output and signal handler.

```cpp
auto *tHandler = TracerHandler::getTracer(); // Get the singelton
auto  cfg      = tHandler->getConfig();      // Get the current config
// Edit cfg

tHandler->setConfig(cfg);                    // Update the config
auto printer = PrinterContainer::fancy();    // Generates a printer
// Edit printer config

tHandler->setup(std::move(printer));         // Sets things up. Now the signal handler is setup
```

# Status

## Supported Operating Systems

 - Linux
 - Windows

It currently does not work on Mac OS because of ASLR. Pull requests are welcome :D.

FreeBSD is currently not tested.

## Backend status

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
