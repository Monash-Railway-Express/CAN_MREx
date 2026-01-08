# VeDirectFrameHandler

This is a Library to read from Victron devices using VE.Direct protocol.

The VE.Direct Protocol FAQ is located here: https://www.victronenergy.com/live/vedirect_protocol:faq

The library was tested using a Raspberry PI and a ESP32.

The application passes serial bytes to the library.
The library parses those bytes, verifies the frame, and makes a public buffer available to the application to read as a name/value pair.

It's able to parse TEXT and HEX protocol messages.

## Compiling it

This is a library, you should just include it as you regulary do it.
For example using `cmake`:

```
include(FetchContent)
FetchContent_Declare(VeDirectFrameHandler  GIT_REPOSITORY https://gitlab.womolin.de/martin.verges/VeDirectFrameHandler.git)
FetchContent_MakeAvailable(VeDirectFrameHandler)
include_directories(${vedirectframehandler_SOURCE_DIR})
```
_Hint: Don't forget to add it to your `target_link_libraries`._

If you like to statically include this library using cmake, you have to overwrite the `VEDIRECT_BUILD_STATIC` variable with `true`.

# License

This library is based on a publically released reference implementation by Victron.
Victron only released the framehandler.cpp portion, so some assumptions were made to adapt it.

Copyright (c) 2019 Victron Energy BV
Portions Copyright (c) 2020 Chris Terwilliger
Portions Copyright (C) 2022 Martin Verges
