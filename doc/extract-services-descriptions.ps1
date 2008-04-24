if( $args.length -lt 2 )
{
	write-error "Not enough arguments"
	exit
}

$pattern = "(?<=/\*!)(?:(?:.*?(?=\s*\*/\s*$))|(?:(?:(?!\s*\*/\s*$).)*$))"

$output = $args[0]
remove-item -path $output -erroraction SilentlyContinue

$doctype = "<!DOCTYPE x [<!ENTITY nbsp '&#160;'><!ENTITY ndash '&#8211;'><!ENTITY mdash '&#8212;'>]>"
add-content -path $output -value $doctype
add-content -path $output -value "<services>"

[Reflection.Assembly]::LoadWithPartialName("System.Web") | out-null

for( $i = 1; $i -lt $args.length; ++$i )
{
	$group = "<group name=`"" + [web.httputility]::HtmlEncode( (split-path -leaf -path $args[$i]) ) + "`">"
	add-content -path $output -value $group
	
	$lines = get-content -path $args[$i]
	foreach( $line in $lines )
	{
		if( $line -match $pattern )
		{
			add-content -path $output -value $matches[0]
		}
	}
	
	add-content -path $output -value "</group>"
}

add-content -path $output -value "</services>"
