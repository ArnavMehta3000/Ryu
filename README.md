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

## Configuration

`--ryu-build-with-editor=[y|n]` Enable the game editor

`--ryu-throw-on-fail-hresult=[y|n]` Assert on failure of HRESULT (default: y)

`--ryu-enable-tracy-profiling=[y|n]` Enable profiling via Tracy (default: n)

`--ryu-unity-build=[y|n]` Compile using unity build (default: y)

`--ryu-log-level=RYU-LOG-LEVEL` Set the log level

`--ryu-enable-hot-reload=[y|n]` Enable hot-reload for game code (requires editor) (default: n)

These configs can be set using the project generator in extended mode or the following command:

```bash
xmake f <config=value>
```

## Generating Project Files

- Run the `GenerateProjectFiles.bat` to create a Visual Studio solution in the `build` folder with only the `debug` configuration
- Run `GenerateProjectFiles.bat x` (pass 'x' as an argument) to run the project generator in extended mode to allow further custom configuration

## Building

Run the following command to build all targets

```bash
xmake
```

All Ryu's modules/targets (in xmake terms) are prefixed with 'Ryu', external modules will not have that prefix. To see all the available targets run:

```bash
xmake show -l targets
```

Optionally only build a target

```bash
xmake build <target_name>
```

## Ryu Config

Ryu identifies its root directory by looking for the `Ryu.toml` configuration file. If this file is not found, Ryu cannot be launched. Currently it is configured to run the Testbench project and by default will build in `debug` mode

- Paths
	- `ProjectDir` - Which project directory to look for assets/scripts/config files (defines the active project). This is relative to the engine root directory
	- `GameDLLName` - Project DLL name can differ from project name. This can be ignored if building the game as standalone, but is required if `--ryu-build-with-editor` is enabled

## The Testbench

The Testbench is a dummy game project that is used for testing purposes. By default Testbench is run as a standalone executable. But if `--ryu-build-with-editor=y` is set, then it will be built as a DLL. In such a scenario, the editor project will be built and be used as the executable to run.

### Testbench Config

The Testbench project has a couple config `.toml` files.

#### App Config

- Logging
	- `EnableLogToConsole` - Opens a console logger even in release builds
	- `EnableLogToFile` - Opens a log output file
	- `ForceLogToOutput` - Force log messages to debug output even when a debugger is not attached (eg. VS Output Window)
	- `LogFilePath` - String to the output log file relative to the executable directory

#### Graphics Config

- Debugging
	- `EnableDebugLayer` - Enables graphics debug layer (only in debug builds)
	- `EnableGPUBasedValidation` - Enabled GPU based validation, requires debug layer to be enabled (only in debug builds)
- Rendering
	- `AllowTearing` - Allow swapchain screen tearing
	- `EnableVSync` - Enable VSync
	- `UseWARP` - Use (or fallback to) WARP renderer

## Testing

To run the automated tests that check project compilation state. Execute the following command

```bash
xmake test
```
