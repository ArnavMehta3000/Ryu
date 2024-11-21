# Ryu Engine

Ryu Engine is an experimentation playground for making game engines<br>

[![](https://tokei.rs/b1/github/ArnavMehta3000/Ryu?category=lines)](https://github.com/ArnavMehta3000/Ryu)
[![](https://tokei.rs/b1/github/ArnavMehta3000/Ryu?category=code)](https://github.com/ArnavMehta3000/Ryu)

# Requirements

- C++ 23
- [Xmake build system](https://xmake.io/)

# Building

> [!NOTE]
> - Ryu Engine only builds on Windows x64 and has only been tested with MSVC/Visual Studio 2022
> - All xmake commands are run from the root folder (of the repository)

- To build Ryu you **need to ensure** you have the xmake build system installed and added to you path.

## Build All Default Projects

To build the default projects of Ryu (and their dependencies) simply run xmake.
Additionally provided `.bat` scripts can be run (from repo root directory) in the [Scripts folder](/Scripts/)

```bash
xmake
```

By default xmake will build Ryu in `debug` configuration. This can be changed by running the following command **before** running any build command. Optionally provided `Scripts/*.bat` can be used.

```bash
xmake f -m <debug|release>
```

## Build a Specifc Project

To build a specific project run the build command with the project name

```bash
xmake build <project name>
```

## Generating IDE Project Files

IDE files for are generated with the help of [xmake](https://xmake.io/). To see how to generate IDE specific project files using xmake [click here](https://xmake.io/#/plugin/builtin_plugins).

## Generating Documentation

Documentation can be generated (using doxygen). Simply run [GenerateDocs.bat](/Scripts/GenerateDocs.bat) from the repo root folder.

Documentation will be generated in the `./build/documentation/` folder

# Architecture

Ryu is architected in a very modular way which makes it easy to extend and modify.

To see a list of all the projects (referred to as *targets* by xmake), you can run the following command

```bash
xmake show -l targets
```

- All of the targets build into a static library. Except the `RyuEditor` and `RyuTestbench`. They build into executables.
- `RyuCore` is a phony target which will build all core targets
- `RyuEngine` is a special static library where it merge archives all dependent static libraries into a single `RyuEngine.lib`
- `RyuTestbench` is a test 'game' project where current engine functionality is being tested. This project may or may not always compile

## Dependencies

The packages can be seen in the [Packages.lua](Xmake/Packages.lua) file
- [fmt](https://github.com/fmtlib/fmt)
- [DirectX-Headers](https://github.com/microsoft/DirectX-Headers)
- [imgui](https://github.com/ocornut/imgui/tree/docking)
- [libassert](https://github.com/jeremy-rifkin/libassert)
- [uuid_v4](https://github.com/crashoz/uuid_v4)
- [entt](https://github.com/skypjack/entt)
- [Angelscript](https://www.angelcode.com/angelscript/) (addons included in the repo)

## Testing

To run the automated tests that check project compilation state. Execute the following command

```bash
xmake test
```
