WIX Extension - reading properties from XML file
------------------------------------------------

motivation
----------

Installation state is written to wolframe.xml file.
It is nice to be able to get the values from wolframe.xml
again when doing a repair/upgrade in the installer. Also
we can avoid to use the registry to store settings which
then get out of date relative to the XML configuration file.

Based on the project on http://wixcontrib.codeplex.com/,
but using it without Visual Studio stuff. Also simplified
and flattened the layout, removed precompiled header stuff.
Removed XML-build-files, we use NMAKE.
