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
	<xsl:param name="pluginVersion" select="'x.x.x'"/>
	<xsl:param name="lastCopyrightYear" select="'9999'"/>
	<xsl:param name="servicesPath" select="'services.xml'"/>

	<xsl:variable name="services"
		select="document($servicesPath)/services"/>
	<xsl:variable name="listOfServicesHeaderId"
		select="generate-id(//htm:h1[normalize-space(.)='List of services'][1])"/>

	<!-- copy input to output -->
	<xsl:template match="node()|@*">
		<xsl:copy xml:space="default">
			<xsl:apply-templates select="node()|@*"/>
		</xsl:copy>
	</xsl:template>

	<!-- copy the style without escaping the contents -->
	<xsl:template match="//htm:style">
		<xsl:copy>
			<!-- let attributes be copied -->
			<xsl:apply-templates select="@*"/>
			<xsl:value-of select="." disable-output-escaping="yes"/>
		</xsl:copy>
	</xsl:template>

	<xsl:template match="node()[@id='pluginVersion']">
		<xsl:value-of select="$pluginVersion"/>
	</xsl:template>

	<xsl:template match="node()[@id='lastCopyrightYear']">
		<xsl:value-of select="$lastCopyrightYear"/>
	</xsl:template>

	<!-- Prepare H1 elements, except Table of contents -->
	<xsl:template match="//htm:h1[normalize-space(.)!='Table of contents']">
		<xsl:copy>
			<!-- add id attribute -->
			<xsl:attribute name="id">
				<xsl:value-of select="generate-id()"/>
			</xsl:attribute>
			<!-- let other attributes be copied -->
			<xsl:apply-templates select="@*"/>
			<!-- insert number -->
			<xsl:value-of select="1 + count(preceding-sibling::htm:h1[normalize-space(.)!='Table of contents'])"/>
			<xsl:text>. </xsl:text>
			<!-- insert text content -->
			<xsl:value-of select="normalize-space(.)"/>
		</xsl:copy>
	</xsl:template>

	<!-- Generate table of contents -->
	<xsl:template match="node()[@id='tableOfContents']">
		<xsl:copy>
			<!-- let attributes be copied -->
			<xsl:apply-templates select="@*"/>
			<!-- add table of contents entries -->
			<xsl:for-each select="//htm:h1[normalize-space(.)!='Table of contents']">
				<li>
					<a href="#{generate-id()}">
						<xsl:value-of select="normalize-space(.)"/>
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
			<xsl:for-each select="$services/group">
				<xsl:sort select="@name" order="ascending"/>
				<xsl:for-each select="service">
					<xsl:sort select="@name" order="ascending"/>
					<li>
						<a href="#{generate-id()}">
							<xsl:value-of select="normalize-space(@name)"/>
						</a>
					</li>
				</xsl:for-each>
			</xsl:for-each>
		</xsl:copy>
	</xsl:template>

	<!-- Generate descriptions of services -->
	<xsl:template match="node()[@id='servicesDescriptions']">
		<xsl:copy>
			<!-- let attributes be copied -->
			<xsl:apply-templates select="@*"/>
			<!-- add descriptions -->
			<xsl:for-each select="$services/group">
				<xsl:sort select="@name" order="ascending"/>
				<xsl:for-each select="service">
					<xsl:sort select="@name" order="ascending"/>
					<li id="{generate-id()}">
						<div>
							<xsl:value-of select="normalize-space(@name)"/>
							<xsl:text>( </xsl:text>
							<xsl:for-each select="argument">
								<xsl:value-of select="normalize-space(@name)"/>
								<xsl:if test="last() > position()">
									<xsl:text>, </xsl:text>
								</xsl:if>
							</xsl:for-each>
							<xsl:text> )</xsl:text>
						</div>
						<a href="#{$listOfServicesHeaderId}">Go up</a>

						<p>
							<xsl:apply-templates select="description"/>
						</p>

						<xsl:if test="count(requirements) > 0">
							<h4>Requirements</h4>
							<p>
								<xsl:value-of select="normalize-space(requirements)"/>
							</p>
						</xsl:if>

						<xsl:if test="count(argument) > 0">
							<h4>Parameters</h4>
							<dl>
								<xsl:for-each select="argument">
									<dt>
										<xsl:value-of select="normalize-space(@type)"/>
										<xsl:text> </xsl:text>
										<span>
											<xsl:value-of select="normalize-space(@name)"/>
										</span>
										<xsl:if test="@optional='true'">&#160;&#160;&#160;optional</xsl:if>
									</dt>
									<dd>
										<xsl:apply-templates select="self::node()"/>
									</dd>
								</xsl:for-each>
							</dl>
						</xsl:if>

						<h4>Return value</h4>
						<xsl:choose>
							<xsl:when test="count(return-value) > 0">
								<dl>
									<dt>
										<xsl:value-of select="normalize-space(return-value/@type)"/>
									</dt>
									<dd>
										<xsl:apply-templates select="return-value"/>
									</dd>
								</dl>
							</xsl:when>
							<xsl:otherwise>
								<p>No return value.</p>
							</xsl:otherwise>
						</xsl:choose>
					</li>
				</xsl:for-each>
			</xsl:for-each>
		</xsl:copy>
	</xsl:template>

	<!-- Copy description/argument/return-value text contents creating <br/> tags as indicated in the source -->
	<xsl:template match="description|argument|return-value">
		<xsl:for-each select="child::node()">
		<xsl:choose>
			<xsl:when test="local-name()='br'">
				<br />
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="normalize-space()"/>
			</xsl:otherwise>
		</xsl:choose>
		</xsl:for-each>
	</xsl:template>
	
</xsl:stylesheet>
