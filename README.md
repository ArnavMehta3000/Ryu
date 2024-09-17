# Ryu Engine

Ryu Engine is an experimentation playground for making game engines<br>

[![](https://tokei.rs/b1/github/ArnavMehta3000/Ryu?category=lines)](https://github.com/ArnavMehta3000/Ryu)
[![](https://tokei.rs/b1/github/ArnavMehta3000/Ryu?category=code)](https://github.com/ArnavMehta3000/Ryu)

# Building

> [!NOTE]
> Ryu Engine only builds on Windows x64 and has only been tested with MSVC/Visual Studio 2022

## Requirements

- C++ 23
- [Xmake build system](https://xmake.io/)

## Dependencies

- [spdlog](https://github.com/gabime/spdlog)
- [backward-cpp](https://github.com/bombela/backward-cpp)
- [nlohmann-json](https://github.com/nlohmann/json)

## Configuration

Before building you can configure certain compile time settings.

### Engine Configuration

The `EngineConfig.toml` file is located in the [Config](/Config/EngineConfig.toml) folder. Here you can modify engine plugins. See the file itself on how to do that.

[OPTIONAL] If you choose to add a new plugin that is used by the engine. Make sure to add the same plugin name to the [engine `xmake.lua` file](/Ryu/Engine/xmake.lua) in the `engine_plugins` table to add that plugin. This will ensure the plugin directory is added to the engine include directory along with it being a build dependency (does not link the DLL). Otherwise you'll have to manually compile your plugin (to ensure the DLL is up to date).

### Log Verbosity

Using the following command you can override the log verbosity at compile time

```bash
xmake f --log-level=<none|warn|info|debug|trace>
```

Log verbosity defaults to `Trace` in debug builds and `Info` in release builds

### Console

Using the following command you can enable or disable the creation of a log console. 

```bash
xmake f --console=<true|false>
```

### Mode

You can set the build mode to debug or release using the following command

```bash
xmake f -m <debug|release>
```

### Example

This is an example command to configure to build project in release mode with log debug log verbosity with an attached console

```bash
xmake f -m debug --log-level=debug --console=true
```

# Building

To build the project simply run the following command in the repository folder

```bash
xmake
```
