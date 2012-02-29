WIX Extension - reading properties from XML file
================================================

   Motivation
   References for Developers
   
Motivation
----------

Installation state is written to wolframe.xml file.
It is nice to be able to get the values from wolframe.xml
again when doing a repair/upgrade in the installer. Also
we can avoid to use the registry to store settings which
then get out of date in respect to the XML configuration file.

The code here is based on the WixContrib project on
http://wixcontrib.codeplex.com/, but using it without Visual
Studio stuff. Also simplified and flattened the layout,
removed precompiled header stuff.
Removed XML-build-files, we use NMAKE.

References for Developers
-------------------------

* Some general links:
  http://blogs.technet.com/b/alexshev/archive/2009/05/27/from-msi-to-wix-part-25-installable-items-updating-xml-files-using-xmlfile.aspx
  http://wix.sourceforge.net/manual-wix2/wix_xsd_xmlfile.htm
  https://github.com/apache/subversion/blob/57b9abdac726d701db7b2f89609bfb8e6397b3ef/packages/windows-WiX/BuildSubversion/Setup.wxs
  http://weblogs.asp.net/lorenh/archive/2005/10/13/427398.aspx
  http://www.slideshare.net/alekdavis/beginners-guide-to-windows
  http://wix.tramontana.co.hu/tutorial/user-interface-revisited
* Remember values in a repair/update:
  http://robmensching.com/blog/posts/2010/5/2/The-WiX-toolsets-Remember-Property-pattern
  http://stackoverflow.com/questions/3449770/how-do-i-read-app-config-values-and-set-properties-in-wix
  http://blogs.technet.com/b/alexshev/
  http://netinverse.com/devblogs/wix/read-configuration-parameters-from-xml-file/355/
