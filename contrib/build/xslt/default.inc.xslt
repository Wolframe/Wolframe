<?xml version="1.0" encoding="ISO-8859-1" ?>
<xsl:stylesheet xmlns="http://www.w3.org/1999/xhtml"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

<!-- catch all template, from "EduTech Wiki" -->

  <xsl:template match="*">
    <dl>
      <dt>Untranslated node:
        <strong><xsl:value-of select="name()"/></strong>
      </dt>
      <dd>
        <xsl:copy>
          <xsl:apply-templates select="@*"/>
          <xsl:apply-templates select="node()"/>
        </xsl:copy>
      </dd>
    </dl>
  </xsl:template>

  <xsl:template match="text()|@*">
    Contents: <xsl:value-of select="."/>
  </xsl:template>

</xsl:stylesheet>
