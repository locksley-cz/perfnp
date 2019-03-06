###########################
## Appveyor Entry points ##
###########################

function DoCoverage {
  return $env:CONFIGURATION -eq "Debug" -and $env:PLATFORM -eq "x64"
}

function AppveyorInstall {
  if (DoCoverage) {
    Invoke-Expression-Safely "python -m pip --disable-pip-version-check install codecov"
    installOpenCppCoverage
  }
}


function AppveyorConfigure {
  if (DoCoverage) {
    Invoke-Expression-Safely "cmake '-H.' '-Bbuild' '-A$env:PLATFORM' '-DMEMORYCHECK_COMMAND=misc\appveyor\coverage.bat' '-DMEMORYCHECK_COMMAND_OPTIONS=--@' '-DMEMORYCHECK_TYPE=Valgrind'"
  } else {
    Invoke-Expression-Safely "cmake '-H.' '-Bbuild' '-A$env:PLATFORM'"
  }
}


function AppveyorTests {

  Set-Location "build"
  try {
    if (DoCoverage) {
      Invoke-Expression-Safely "ctest -j 2 -C '$env:CONFIGURATION' -D ExperimentalMemCheck"
      MergeCoverage
    } else {
      Invoke-Expression-Safely "ctest -j 2 -C '$env:CONFIGURATION'"
    }

  } finally {
    Set-Location ".."
  }

  if (DoCoverage) {
    Invoke-Expression-Safely "codecov --root . --no-color --disable gcov -f build\cobertura.xml"
  }
}



###################
## Various Tools ##
###################


# Invokes a command and throws an exception if return value is not 0
function Invoke-Expression-Safely($command) {
  Invoke-Expression "$command"
  if ("$LastExitCode" -ne 0) {
    throw "Failed to execute command: $command"
  }
}


# Installs the OpenCppCoverage and updates the PATH
function installOpenCppCoverage {
  # Downloads are done from the oficial github release page links
  $downloadUrl = "https://github.com/OpenCppCoverage/OpenCppCoverage/releases/download/release-0.9.6.1/OpenCppCoverageSetup-x64-0.9.6.1.exe"
  $installerPath = [System.IO.Path]::Combine($Env:USERPROFILE, "Downloads", "OpenCppCoverageSetup.exe")

  if(-Not (Test-Path $installerPath)) {
      Write-Host -ForegroundColor White ("Downloading OpenCppCoverage from: " + $downloadUrl)
      Start-FileDownload $downloadUrl -FileName $installerPath
  }

  Write-Host -ForegroundColor White "About to install OpenCppCoverage..."

  $installProcess = (Start-Process $installerPath -ArgumentList '/VERYSILENT' -PassThru -Wait)
  if($installProcess.ExitCode -ne 0) {
      throw [System.String]::Format("Failed to install OpenCppCoverage, ExitCode: {0}.", $installProcess.ExitCode)
  }

  # Assume standard, boring, installation path of ".../Program Files/OpenCppCoverage"
  $installPath = [System.IO.Path]::Combine(${Env:ProgramFiles}, "OpenCppCoverage")
  $env:Path="$env:Path;$installPath"
}


# Merge all 'cov-report*.bin' coverage reports into the 'cobertura.xml' file
function MergeCoverage {
  Set-Variable -Name command -Value "OpenCppCoverage --quiet --export_type=cobertura:cobertura.xml"
  foreach ($file in Get-ChildItem -File -Name -Recurse -Filter 'MemoryChecker.*.log.bin') {
      Set-Variable -Name command -Value "$command --input_coverage $file"
  }
  Invoke-Expression-Safely "$command"
}
