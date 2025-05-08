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
    xmake project -k vsxmake -m debug build
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

    # RYU logging options
    $config.ryuDebug = Get-UserChoice -Prompt "Enable debug logs? (y/n)" -OptionName "Debug logs"
    $config.ryuWarn = Get-UserChoice -Prompt "Enable warning logs? (y/n)" -OptionName "Warning logs"
    $config.ryuTrace = Get-UserChoice -Prompt "Enable trace logs? (y/n)" -OptionName "Trace logs"
    $config.ryuInfo = Get-UserChoice -Prompt "Enable info logs? (y/n)" -OptionName "Info logs"

    # Assert options
    $config.assertOnFail = Get-UserChoice -Prompt "Throw exceptions on HRESULT failure? (y/n)" -OptionName "Throw exceptions on HRESULT failure"

    # Profiling options
    $config.enableProfiling = Get-UserChoice -Prompt "Enable Tracy profiling? (y/n)" -DefaultValue "n" -OptionName "Tracy profiling"

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
        --ryu-log-debug-enabled=$($Config.ryuDebug) `
        --ryu-log-warn-enabled=$($Config.ryuWarn) `
        --ryu-log-trace-enabled=$($Config.ryuTrace) `
        --ryu-log-info-enabled=$($Config.ryuInfo) `
        --ryu-throw-on-fail-hresult=$($Config.assertOnFail) `
        --ryu-enable-tracy-profiling=$($Config.enableProfiling)
}

# Function to generate project files
function Generate-ProjectFiles
{
    param(
        [string]$ModeString,
        [string]$OutputDir
    )

    Write-Host "`nGenerating project files with modes: $ModeString and output directory: $OutputDir" -ForegroundColor Cyan
    xmake project -k vsxmake -m "$ModeString" $OutputDir

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
