<?xml version="1.0" encoding="ISO-8859-1" ?>
<xsl:stylesheet xmlns="http://www.w3.org/1999/xhtml"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  
  <xsl:output method="html" version="1.1" encoding="UTF-8" indent="no"/>

  <xsl:include href="page.inc.xslt"/>

  <xsl:template match="/page">
    <xsl:call-template name="page">
      <xsl:with-param name="base" select="/page/@base"/>
      <xsl:with-param name="title" select="'Build Result'"/>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="log">
    <h2>Build Result&#160;<xsl:value-of select="revision"/>&#160;<xsl:value-of select="arch"/>&#160;<xsl:value-of select="platform"/></h2>
    <table>
      <tr>
        <td class="label">OSC version:
        </td>
        <td>
          <xsl:value-of select="revision"/>
        </td>
      </tr>
      <tr>
        <td class="label">Architecture:
        </td>
        <td>
          <xsl:value-of select="arch"/>
        </td>
      </tr>
      <tr>
        <td class="label">Platform:
        </td>
        <td>
          <xsl:value-of select="platform"/>
        </td>
      </tr>
      <tr>
        <td class="label">OSB build state:
        </td>
        <xsl:variable name="orig_status" select="status"/>
        <xsl:variable name="status">
          <xsl:choose>
            <xsl:when test="$orig_status='succeeded'">ok</xsl:when>
            <xsl:when test="$orig_status='failed'">fail</xsl:when>
          </xsl:choose>
        </xsl:variable>
        <xsl:element name="td">
          <xsl:attribute name="class">status_<xsl:value-of select="$status"/></xsl:attribute>
          <xsl:value-of select="$status"/>
        </xsl:element>
      </tr>
      <tr>
        <td class="label">Download raw logfile:
        </td>
        <td>
          <xsl:element name="a">
            <xsl:attribute name="href">
              <xsl:value-of select="/page/@base"/>docs/<xsl:value-of select="revision"/>/<xsl:value-of select="arch"/>/<xsl:value-of select="platform"/>/log.txt
            </xsl:attribute>
            logfile
          </xsl:element>
        </td>
      </tr>
    </table>
    <pre>
      <xsl:value-of select="tail"/>
    </pre>
  </xsl:template>

</xsl:stylesheet>
