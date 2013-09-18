<?xml version="1.0" encoding="ISO-8859-1" ?>
<xsl:stylesheet xmlns="http://www.w3.org/1999/xhtml"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  
  <xsl:output method="html" version="1.1" encoding="UTF-8" indent="no"/>

  <xsl:include href="page.inc.xslt"/>

  <xsl:template match="/page">
    <xsl:call-template name="page">
      <xsl:with-param name="base" select="/page/@base"/>
      <xsl:with-param name="title" select="'Wolfzilla error'"/>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="error">
    <h2>Daily Builds Error</h2>
    <h3><xsl:value-of select="."/></h3>
  </xsl:template>

  <xsl:template match="trace">
    <div id="trace"><xsl:value-of select="."/></div>
  </xsl:template>

</xsl:stylesheet>

  
