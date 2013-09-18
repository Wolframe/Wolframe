<?xml version="1.0" encoding="ISO-8859-1" ?>
<xsl:stylesheet xmlns="http://www.w3.org/1999/xhtml"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

<xsl:template name="header">
  <xsl:param name="base" select="/page/@base"/>
  <xsl:param name="title"/>

  <head>
    <meta http-equiv="X-UA-Compatible" content="IE=8; IE=9; IE=10"/>
    <link rel="stylesheet" type="text/css" href="{$base}css/master.css"/>
    <title><xsl:value-of select="$title"/></title>
  </head>

</xsl:template>

</xsl:stylesheet>
