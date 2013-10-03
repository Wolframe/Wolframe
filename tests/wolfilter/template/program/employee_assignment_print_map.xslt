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
