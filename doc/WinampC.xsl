<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet
	version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns="http://www.w3.org/1999/xhtml"
	xmlns:htm="http://www.w3.org/1999/xhtml"
	exclude-result-prefixes="htm">

	<xsl:output
		media-type="application/xhtml+xml"
		encoding="utf-8"
		indent="yes"
		omit-xml-declaration="yes"
		method="xml"
		doctype-public="-//W3C//DTD XHTML 1.1//EN"
		doctype-system="http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd"/>

	<!-- declare parameters and their default values -->
	<xsl:param name="lastCopyrightYear" select="'9999'"/>
	<xsl:param name="servicesPath" select="'services.xml'"/>

	<xsl:variable
		name="services"
		select="document($servicesPath)/services"/>

	<!-- copy input to output -->
	<xsl:template match="*|@*">
		<xsl:copy>
			<xsl:apply-templates select="node()|@*"/>
		</xsl:copy>
	</xsl:template>

	<xsl:template match="node()[@id='pluginVersion']">
		<xsl:value-of select="$services/@version"/>
	</xsl:template>

	<xsl:template match="node()[@id='lastCopyrightYear']">
		<xsl:value-of select="$lastCopyrightYear"/>
	</xsl:template>

	<!-- Prepare H1 elements, except Table of contents -->
	<xsl:template match="//htm:h1[.!='Table of contents']">
		<xsl:copy>
			<!-- add id attribute -->
			<xsl:attribute name="id">
				<xsl:value-of select="generate-id()"/>
			</xsl:attribute>
			<!-- let other attributes be copied -->
			<xsl:apply-templates select="@*"/>
			<!-- insert number -->
			<xsl:value-of select="1 + count(preceding-sibling::htm:h1[.!='Table of contents'])"/>
			<xsl:text>. </xsl:text>
			<!-- insert text content -->
			<xsl:value-of select="."/>
		</xsl:copy>
	</xsl:template>

	<!-- Generate table of contents -->
	<xsl:template match="node()[@id='tableOfContents']">
		<xsl:copy>
			<!-- let attributes be copied -->
			<xsl:apply-templates select="@*"/>
			<!-- add table of contents entries -->
			<xsl:for-each select="//htm:h1[.!='Table of contents']">
				<li>
					<a href="#{generate-id()}">
						<xsl:value-of select="."/>
					</a>
				</li>
			</xsl:for-each>
		</xsl:copy>
	</xsl:template>

	<!-- Generate list of services -->
	<xsl:template match="node()[@id='listOfServices']">
		<xsl:copy>
			<!-- let attributes be copied -->
			<xsl:apply-templates select="@*"/>
			<!-- add list of services -->
			<xsl:for-each select="$services/service">
				<xsl:sort select="@name" order="ascending"/>
				<li>
					<a href="#{generate-id()}">
						<xsl:value-of select="@name"/>
					</a>
				</li>				
			</xsl:for-each>
		</xsl:copy>
	</xsl:template>

	<!-- Generate descriptions of services -->
	<xsl:template match="node()[@id='descriptionsOfServices']">
		<xsl:for-each select="$services/service">
			<xsl:sort select="@name" order="ascending"/>
			<div class="serviceDescription" id="{generate-id()}">
				<div class="serviceDefinition">
					<xsl:value-of select="@name"/>
					<xsl:text> ( </xsl:text>
					<xsl:for-each select="argument">
						<xsl:value-of select="@name"/>
					</xsl:for-each>
					<xsl:text> )</xsl:text>
				</div>
				<div class="serviceDescriptionText">
					<xsl:value-of select="description"/>
				</div>

				<xsl:if test="count(argument) > 0">
					<h4>Parameters</h4>
					<dl class="serviceArgumentsList">
						<xsl:for-each select="argument">
							<dt>
								<xsl:value-of select="@name"/>
							</dt>
							<dd>
								<xsl:choose>
									<xsl:when test="@optional='true'">
										[optional]
									</xsl:when>
									<xsl:otherwise>
										[required]
									</xsl:otherwise>
								</xsl:choose>
								[<xsl:value-of select="@type"/>]
								<xsl:value-of select="."/>
							</dd>
						</xsl:for-each>
					</dl>
				</xsl:if>

				<h4>Return value</h4>
				<div class="serviceDescriptionText">
					<xsl:choose>
						<xsl:when test="count(return-value) > 0">
							[<xsl:value-of select="return-value/@type"/>]
							<xsl:value-of select="return-value"/>
						</xsl:when>
						<xsl:otherwise>
							No return value.
						</xsl:otherwise>
					</xsl:choose>
				</div>
			</div>
		</xsl:for-each>
	</xsl:template>
	
</xsl:stylesheet>
