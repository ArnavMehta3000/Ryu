# Ryu Engine

Ryu Engine is an experimentation playground for making game engines

# Building

> [!CAUTION]
> Ryu Engine only builds on Windows x64 and has only been tested with MSVC/Visual Studio 2022

## Requirements

- C++ 23
- [Xmake build system](https://xmake.io/)

## Dependencies

- [spdlog](https://github.com/gabime/spdlog)
- [backward-cpp](https://github.com/bombela/backward-cpp)

## Configuration

Before building you can configure certain compile time settings

### Log Verbosity

Using the following command you can override the log verbosity at compile time

```bash
xmake f --log-level=<none|warn|info|debug|trace>
```

Log verbosity defaults to `Trace` in debug builds and `Info` in release builds

### Console

Using the following command you can enable or disable the creation of a log console. 

```bash
xmake f --console=<y|n>
```

### Mode

You can set the build mode to debug or release using the following command

```bash
xmake f -m <debug|release>
```

### Example

This is an example command to configure to build project in release mode with log debug log verbosity with an attached console

```bash
xmake f -m debug --log-level=debug --console=y
```

# Building

To build the project simply run the following command in the repository folder

```bash
xmake
```