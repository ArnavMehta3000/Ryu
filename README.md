# Ryu Engine

Ryu Engine is an experimentation playground for making game engines<br>

[![](https://tokei.rs/b1/github/ArnavMehta3000/Ryu?category=lines)](https://github.com/ArnavMehta3000/Ryu)
[![](https://tokei.rs/b1/github/ArnavMehta3000/Ryu?category=code)](https://github.com/ArnavMehta3000/Ryu)

## Building

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

The `EngineConfig.json` file is located in the [Config](/Config/EngineConfig.json) folder.
> See the [Plugins](#plugins) section for additonal information.

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
## Plugins

Currently Ryu only supports engine plugins. Support for editor plugins is still WIP.

### Engine Plugins

To add engine plugins, add the target name of the plugin in the [engine config](/Config/EngineConfig.json).

### Plugin Config

Plugins are configured using the `.ryuplugin` file which **should** be in the same directory as the `xmake.lua` file of the plugin.

The plugin config looks something like this and **should** have all the below fields (unless specified to be optional)

The `.ryuplugin` file for the `RyuInput` plugin.
```json
{
  "Plugin": {
    "Version": "1.0.0",
    "Group" : "Ryu/Engine/Plugins",
    "Name": "RyuInput",
    "LoadOrder": "PostInit",
    "TickOrder": "None",
    "RenderOrder": "None",
    "Dependencies": [
      {
        "Name": "RyuCore",
        "CreateLink" : true
      }
    ]
  }
}
```
The file should contain:
- **Plugin:** Root object that defines the plugin
- **Version:** Version of the plugin (as string)
- **Group (optional):** Specified group of the plugin (useful to separate the project into folders in an IDE). Can be left empty or can be removed. Defaults to the ***Plugins*** group
- **Name:** Name of the plugin. This should be the same as the plugin folder and the xmake target name
- **LoadOrder (optional):** Specify the loading order of the plugin. Valid inputs (as strings) are `Default`, `PreInit` or `PostInit`. If left empty (or ignored), the load order defaults to `Default/PostInit`
- **TickOrder (optional):** Specify the ticking order of this plugin. Valid inputs (as strings) are `None`, `Default`, `PreUpdate` or `PostUpdate`. If left empty or ignored, the tick order defaults to `Default/None`
- **RenderOrder (optional):** Specify the render callback order. Valid inputs (as strings) are `None`, `Default`, `PreRender` or `PostRender`. If left empty or ignored, the render order defaults to `Default/None`
- **Dependencies (optional):** Array of other xmake targets this plugin depends on. ***[Recommended]** All plugins should depend on `RyuCore` to get access to Ryu's API.*
  - **Dependencies->Name:** The name of the xmake target this plugin depends on
  - **Dependencies->CreateLink:** Whether or not to link with the dependency library. A boolean `true/false`

> [!NOTE]
> Here is what the default values translate to
> - **Default Load Order:** PostInit
> - **Default Tick Order:** None (this plugin does tick/update)
> - **Default Render Order:** None (this plugin does render)

## Building

To build the project simply run the following command in the repository folder

```bash
xmake build -a
```

## Testing

To run the automated tests. Execute the following command

```bash
xmake test
```