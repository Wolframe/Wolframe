Build Instructions
==================

Requirements
------------

* Wix 3.5

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

* Set the location of the correct VC merge modules in 'Makefile.W32' in
  VDREDIST_MERGE_MODULE

* Set the location of the WIX toolkit in 'Makefile.W32' in variable 'WIX_DIR'.

* Copy prerequisites in src, e. g.

  cd src
  nmake /nologo /f Makefile.W32 ^
     WITH_SSL=1 WITH_EXPECT=1 WITH_LUA=1 WITH_QT=1 ^
     WITH_SQLITE3=1 WITH_PGSQL=1 WITH_LIBXML2=1 ^
     WITH_LIBXSLT=1 WITH_EXAMPLES=1 ^
     copy_prereq

* Build 'wolframe.msi' in the local 'windows' directory

  nmake /nologo /f Makefile.W32 ^
     WITH_SSL=1 WITH_EXPECT=1 WITH_LUA=1 WITH_QT=1 ^
     WITH_SQLITE3=1 WITH_PGSQL=1 WITH_LIBXML2=1 ^
     WITH_LIBXSLT=1 WITH_EXAMPLES=1

* a standalone setup.exe can be build with

  nmake /nologo /f Makefile.W32 ^
     WITH_SSL=1 WITH_EXPECT=1 WITH_LUA=1 WITH_QT=1 ^
     WITH_SQLITE3=1 WITH_PGSQL=1 WITH_LIBXML2=1 ^
     WITH_LIBXSLT=1 WITH_EXAMPLES=1 ^
     setup.exe

More Links
----------

http://blogs.technet.com/b/alexshev/archive/2009/05/27/from-msi-to-wix-part-25-installable-items-updating-xml-files-using-xmlfile.aspx
http://wix.sourceforge.net/manual-wix2/wix_xsd_xmlfile.htm
https://github.com/apache/subversion/blob/57b9abdac726d701db7b2f89609bfb8e6397b3ef/packages/windows-WiX/BuildSubversion/Setup.wxs
http://weblogs.asp.net/lorenh/archive/2005/10/13/427398.aspx
http://www.slideshare.net/alekdavis/beginners-guide-to-windows
http://wix.tramontana.co.hu/tutorial/user-interface-revisited
http://robmensching.com/blog/posts/2010/5/2/The-WiX-toolsets-Remember-Property-pattern
http://stackoverflow.com/questions/3449770/how-do-i-read-app-config-values-and-set-properties-in-wix
http://blogs.technet.com/b/alexshev/
