<?xml version='1.0'?>
<xsl:stylesheet
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	version="1.0">

<xsl:import href="http://docbook.sourceforge.net/release/xsl/current/xhtml/docbook.xsl"/>

<xsl:output  method="xml"/>

	<xsl:param name="html.stylesheet" select="'wolframe.css'"/>
	<xsl:param name="section.autolabel" select="1"/>
	<xsl:param name="section.label.includes.component.label" select="1"/>
	<xsl:param name="section.autolabel.max.depth" select="2"/>

	<xsl:param name="titlepage.pubdate" select="1"/>

</xsl:stylesheet>
<!--	stringparam user.header.content "<a href=\"/\">Back to main page</a>" \ -->
