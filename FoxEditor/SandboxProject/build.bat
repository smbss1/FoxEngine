dotnet build -nologo -noconlog -f net45 -flp1:Verbosity=minimal;logfile=AssemblyBuildErrors.log;errorsonly -flp2:Verbosity=minimal;logfile=AssemblyBuildWarnings.log;warningsonly Sandbox.csproj