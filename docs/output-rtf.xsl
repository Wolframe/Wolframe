<?xml version='1.0'?>
<xsl:stylesheet
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	version="1.0">

<xsl:import href="http://docbook.sourceforge.net/release/xsl/current/fo/docbook.xsl"/>

	<xsl:param name="paper.type" select="'A4'"/>
	<xsl:param name="section.autolabel" select="1"/>
	<xsl:param name="section.label.includes.component.label" select="1"/>
	<xsl:param name="section.autolabel.max.depth" select="2"/>
	<xsl:param name="hyphenate" select="'false'"/>
</xsl:stylesheet>
