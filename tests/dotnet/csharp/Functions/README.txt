Create a TLB (Type Library) file from the DLL:

Call from the shell (with MS SDK environment):
> tlbexp bin\Release\Functions.dll

The file with the extension TLB is created in the current directory. This file can be loaded from native C++ as COM interop object.


Inspect a TLB:
Call \Program Files\Microsoft SDKs\Windows\v7.1\Bin\OleView.exe or use the Wolframe interop test program WolfCLR in tests/interop/

Register the Assembly in the GAC (Administrator command line):
Call "\Program Files\Microsoft SDKs\Windows\v7.1\Bin\gacutil.exe"  /if <assemblypath>

Register the TypeLibrary:
Call "C:\Windows\Microsoft.NET\Framework\v4.0.30319\regtlibv12.exe"  <tlbfilepath>

Register the Types in the TypeLibrary:
Call "C:\Windows\Microsoft.NET\Framework\v4.0.30319\regasm.exe"  <assemblypath>

Documentation about COM/.NET interop is gathered in Links.txt



Errors:
MSB3325:
	sn -i Functions.pfx VS_KEY_9013B58D58AE1C9F 
	(Wolframe)

