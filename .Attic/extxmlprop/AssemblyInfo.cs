using System;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using Microsoft.Tools.WindowsInstallerXml;
using Microsoft.Tools.WixContrib.Extensions;

[assembly: AssemblyTitle("WiXContrib Extension")]
[assembly: AssemblyDescription("Windows Installer XML Toolset Contrib Extension")]
[assembly: AssemblyCulture("")]
[assembly: AssemblyProduct("WixContribExtension")]
[assembly: AssemblyCompany("MS")]
[assembly: AssemblyCopyright("Copyright © MS 2008")]
[assembly: AssemblyTrademark("")]

[assembly: CLSCompliant(true)]
[assembly: ComVisible(false)]

[assembly: AssemblyVersion("1.0.0.0")]
[assembly: AssemblyFileVersion("1.0.0.0")]

//[assembly: AssemblyDefaultHeatExtension(typeof(WixContribHeatExtension))]
[assembly: AssemblyDefaultWixExtension(typeof(WixContribExtension))]
