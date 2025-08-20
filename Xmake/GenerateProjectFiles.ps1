param ([string[]]$ArgsFromBat)

# Change to root directory
Set-Location -Path "$PSScriptRoot/.."

# Function to display section headers
function Show-SectionHeader
{
	param(
		[string]$Title
	)

	Write-Host "`n=== $Title ===" -ForegroundColor Green
}

# Function to get user input with default value
function Get-UserChoice {
	param(
		[string]$Prompt,
		[string]$DefaultValue = "y",
		[string]$OptionName = ""
	)

	$defaultDisplay = if ($DefaultValue -eq "y") { "[y]" } else { "[n]" }
	$response = Read-Host "$Prompt ($defaultDisplay)"

	$result = if ([string]::IsNullOrWhiteSpace($response))
	{
		$DefaultValue
	}
	else
	{
		if ($response.ToLower() -eq "y") { "y" } else { "n" }
	}

	if ($result -eq "y" -and -not [string]::IsNullOrWhiteSpace($OptionName))
	{
		Write-Host "  > $OptionName enabled" -ForegroundColor Yellow
	}

	return $result
}

# Function to get log level choice
function Get-LogLevel {
	param(
		[string]$DefaultValue = "default"
	)

	Write-Host "Available log levels:"
	Write-Host "  1. default - Default logging level"
	Write-Host "  2. trace   - Most verbose logging"
	Write-Host "  3. debug   - Debug level logging"
	Write-Host "  4. info    - Info level logging"
	Write-Host "  5. warn    - Warning level logging only"

	$response = Read-Host "Select log level [1-5] or enter level name [$DefaultValue]"

	if ([string]::IsNullOrWhiteSpace($response))
	{
		$logLevel = $DefaultValue
	}
	else
	{
		switch ($response.ToLower())
		{
			"1" { $logLevel = "default" }
			"2" { $logLevel = "trace" }
			"3" { $logLevel = "debug" }
			"4" { $logLevel = "info" }
			"5" { $logLevel = "warn" }
			"default" { $logLevel = "default" }
			"trace" { $logLevel = "trace" }
			"debug" { $logLevel = "debug" }
			"info" { $logLevel = "info" }
			"warn" { $logLevel = "warn" }
			default {
				Write-Host "Invalid selection. Using default log level." -ForegroundColor Yellow
				$logLevel = "default"
			}
		}
	}

	Write-Host "  > Log level set to: $logLevel" -ForegroundColor Yellow
	return $logLevel
}

# Function to get output directory
function Get-OutputDirectory
{
	param(
		[string]$DefaultDirectory = "build"
	)

	$outputDir = Read-Host "Enter output directory [$DefaultDirectory]"

	if ([string]::IsNullOrWhiteSpace($outputDir))
	{
		$outputDir = $DefaultDirectory
		Write-Host "  > Using default output directory: $outputDir" -ForegroundColor Yellow
	}

	return $outputDir
}

# Function to run the default xmake command
function Run-Default
{
	Write-Host "Running default xmake build (debug, build directory)..." -ForegroundColor Cyan
	Write-Host "=> Run in extended mode by passing 'x' as an argument" -ForegroundColor Cyan
	xmake project -k vsxmake -y -m debug build
}

# Function to configure build modes
function Get-BuildModes
{
	Show-SectionHeader "Build Mode Configuration"

	$modes = @()

	if ((Get-UserChoice -Prompt "Enable 'debug' mode? (y/n)" -OptionName "Debug mode") -eq "y")
	{
		$modes += "debug"
	}

	if ((Get-UserChoice -Prompt "Enable 'release' mode? (y/n)" -DefaultValue "n" -OptionName "Release mode") -eq "y")
	{
		$modes += "release"
	}

	if ((Get-UserChoice -Prompt "Enable 'releasedbg' mode? (y/n)" -DefaultValue "n" -OptionName "Release with debug info mode") -eq "y")
	{
		$modes += "releasedbg"
	}

	if ($modes.Count -eq 0) {
		Write-Host "No build modes selected. Adding default 'debug' mode." -ForegroundColor Yellow
		$modes += "debug"
	}

	$modeString = ($modes -join ",")
	Write-Host "Selected modes: $modeString" -ForegroundColor Cyan

	return $modeString
}

# Function to configure project options
function Get-ProjectConfiguration
{
	Show-SectionHeader "Project Configuration Options"

	$config = @{}

	# RYU logging options - now simplified to a single log level
	$config.logLevel = Get-LogLevel -DefaultValue "default"

	# Assert options
	$config.assertOnFail = Get-UserChoice -Prompt "Throw exceptions on HRESULT failure? (y/n)" -OptionName "Throw exceptions on HRESULT failure"

	# Profiling options
	$config.enableProfiling = Get-UserChoice -Prompt "Enable Tracy profiling? (y/n)" -DefaultValue "n" -OptionName "Tracy profiling"

	# Game build style
	$config.withEditor = Get-UserChoice -Prompt "Build with editor (y/n))" -DefaultValue "n" -OptionName "Build editor"
	return $config
}

# Function to apply project configuration
function Apply-ProjectConfiguration
{
	param(
		[hashtable]$Config
	)

	Write-Host "`nApplying project configurations..." -ForegroundColor Cyan

	xmake f `
		--ryu-log-level=$($Config.logLevel) `
		--ryu-throw-on-fail-hresult=$($Config.assertOnFail) `
		--ryu-enable-tracy-profiling=$($Config.enableProfiling) `
		--ryu-build-with-editor=$($Config.withEditor)
}

# Function to generate project files
function Generate-ProjectFiles
{
	param(
		[string]$ModeString,
		[string]$OutputDir
	)

	Write-Host "`nGenerating project files with modes: $ModeString and output directory: $OutputDir" -ForegroundColor Cyan
	xmake project -k vsxmake -y -m "$ModeString" $OutputDir

	Write-Host "`nProject generation complete!" -ForegroundColor Green
}

# Function to run extended mode
function Run-Extended
{
	# Get build modes
	$modeString = Get-BuildModes

	# Get project configuration
	$config = Get-ProjectConfiguration

	# Get output directory
	Show-SectionHeader "Output Configuration"
	$outputDir = Get-OutputDirectory -DefaultDirectory "build"

	# Apply project configuration
	Apply-ProjectConfiguration -Config $config

	# Generate project files
	Generate-ProjectFiles -ModeString $modeString -OutputDir $outputDir
}

# Main logic
function Start-Generator
{
	Write-Host "Ryu Solution Generator" -ForegroundColor Magenta
	Write-Host "======================" -ForegroundColor Magenta

	if ($ArgsFromBat.Count -eq 0)
	{
		Run-Default
	}
	elseif ($ArgsFromBat[0].ToLower() -eq "x")
	{
		Run-Extended
	}
	else {
		Write-Host "Unknown argument passed. Use no argument for default mode or 'x' for extended mode." -ForegroundColor Red
	}
}

# Execute the main function
Start-Generator
