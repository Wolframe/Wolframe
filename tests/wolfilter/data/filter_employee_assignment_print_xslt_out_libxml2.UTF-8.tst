**
**requires:LIBXML2
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assignmentlist><assignment><task><title>job 1</title><key>A123</key><customernumber>324</customernumber></task><task><title>job 2</title><key>V456</key><customernumber>567</customernumber></task><employee><firstname>Julia</firstname><surname>Tegel-Sacher</surname><phone>098 765 43 21</phone></employee><issuedate>13.5.2006</issuedate></assignment><assignment><task><title>job 3</title><key>A456</key><customernumber>567</customernumber></task><task><title>job 4</title><key>V789</key><customernumber>890</customernumber></task><employee><firstname>Jakob</firstname><surname>Stegelin</surname><phone>012 345 67 89</phone></employee><issuedate>13.5.2006</issuedate></assignment></assignmentlist>**config
--input-filter libxml2 --output-filter employee_assignment_print_map --module ../../src/modules/filter/libxml2/mod_filter_libxml2 -c wolframe.conf -

**requires:LIBXSLT
**file:wolframe.conf
Processor
{
	program employee_assignment_print_map.xslt
}
**file:employee_assignment_print_map.xslt
<?xml version="1.0" standalone="yes"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/assignmentlist">
  <assignmentlist>
    <xsl:for-each select="assignment">
      <assignment>
      <xsl:for-each select="task">
	<task>
	<title>title:<xsl:value-of select="title"/></title>
	<key>key:<xsl:value-of select="key"/></key>
	<customernumber>customernumber:<xsl:value-of select="customernumber"/></customernumber>
	</task>
      </xsl:for-each>
      <xsl:for-each select="employee">
	<employee>
	<firstname>firstname:<xsl:value-of select="firstname"/></firstname>
	<surname>surname:<xsl:value-of select="surname"/></surname>
	<phone>phone:<xsl:value-of select="phone"/></phone>
	</employee>
      </xsl:for-each>
      <issuedate>issuedate:<xsl:value-of select="issuedate"/></issuedate>
      </assignment>
    </xsl:for-each>
  </assignmentlist>
</xsl:template>

</xsl:stylesheet>
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assignmentlist><assignment><task><title>title:job 1</title><key>key:A123</key><customernumber>customernumber:324</customernumber></task><task><title>title:job 2</title><key>key:V456</key><customernumber>customernumber:567</customernumber></task><employee><firstname>firstname:Julia</firstname><surname>surname:Tegel-Sacher</surname><phone>phone:098 765 43 21</phone></employee><issuedate>issuedate:13.5.2006</issuedate></assignment><assignment><task><title>title:job 3</title><key>key:A456</key><customernumber>customernumber:567</customernumber></task><task><title>title:job 4</title><key>key:V789</key><customernumber>customernumber:890</customernumber></task><employee><firstname>firstname:Jakob</firstname><surname>surname:Stegelin</surname><phone>phone:012 345 67 89</phone></employee><issuedate>issuedate:13.5.2006</issuedate></assignment></assignmentlist>
**end
