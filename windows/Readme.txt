Windows WIX Packaging Instructions
==================================

    Requirements
    Prepare the build
    Some useful links for the packager
    
Requirements
------------

* Wix 3.5 (which requires .NET 2.0)

Prepare the build
-----------------

* Build Wolframe and all requirements, especially make sure you build them all
  against the same architecture and the same VC runtime!

  - use XN Resource Editor to check the manifest of VC in "XP Theme Manifest"
  - check the manifests of ALL binary artifacts!
  - for more information check:
    - http://wix.sourceforge.net/manual-wix3/install_vcredist.htm
    - http://blogs.msdn.com/b/astebner/archive/2007/02/13/building-an-msi-using-wix-v3-0-that-includes-the-vc-8-0-runtime-merge-modules.aspx
    - http://msdn.microsoft.com/en-us/library/ms235290(v=vs.80).aspx
    - http://social.msdn.microsoft.com/Forums/en-US/vcgeneral/thread/867cf808-bf1f-4f4a-bde1-cd75d92d73ad
    - http://stackoverflow.com/questions/59635/app-does-not-run-with-vs-2008-sp1-dlls-previous-version-works-with-rtm-versions
    - http://stackoverflow.com/questions/59635/app-does-not-run-with-vs-2008-sp1-dlls-previous-version-works-with-rtm-versions

* Set the location of the WIX toolkit in 'makefiles\nmake\config.mk' in variable 'WIX_DIR'.

* Copy prerequisites in 'src' and 'clients\qt' subdirs, e. g.

  cd src
  nmake /nologo /f Makefile.W32 ^
     WITH_SSL=1 WITH_EXPECT=1 WITH_LUA=1 WITH_QT=1 ^
     WITH_SQLITE3=1 WITH_PGSQL=1 WITH_EXAMPLES=1 ^
     copy_prereq
  cd clients\qt
  nmake /nologo /f Makefile.W32 ^
     WITH_SSL=1 WITH_EXPECT=1 WITH_LUA=1 WITH_QT=1 ^
     WITH_SQLITE3=1 WITH_PGSQL=1 WITH_EXAMPLES=1 ^
     copy_prereq

* Build documentation in 'docs' subdir:

  cd docs
  nmake /nologo /f Makefile.W32 doc

* Build 'wolframe.msi' in the local 'windows' directory

  nmake /nologo /f Makefile.W32 ^
     WITH_SSL=1 WITH_EXPECT=1 WITH_LUA=1 WITH_QT=1 ^
     WITH_SQLITE3=1 WITH_PGSQL=1 WITH_EXAMPLES=1

* a standalone setup.exe can be build with

  nmake /nologo /f Makefile.W32 ^
     WITH_SSL=1 WITH_EXPECT=1 WITH_LUA=1 WITH_QT=1 ^
     WITH_SQLITE3=1 WITH_PGSQL=1 WITH_EXAMPLES=1 ^
     setup.exe

Some useful links for the packager
----------------------------------

* merge modules and prerequisites:
  - http://cticoder.wordpress.com/2009/10/24/simulating-vs-nets-msi-installurl-property-with-wix/
* bootstrapper, dotNetInstaller:
  - http://msdn.microsoft.com/en-us/library/ms229223.aspx
  - http://stackoverflow.com/questions/1971407/how-to-silent-run-an-installer-of-another-programpostgresql-during-the-install
  - http://msdn.microsoft.com/en-us/library/aa730839%28VS.80%29.aspx
  - http://lostechies.com/gabrielschenker/2010/05/19/creating-a-bootstrapper-with-dotnetinstaller/
  - http://windows-installer-xml-wix-toolset.687559.n2.nabble.com/How-to-install-prerequisites-based-upon-feature-selection-td5631730.html
