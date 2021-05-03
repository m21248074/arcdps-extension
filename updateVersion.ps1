$file = $args[0]
$regex = '(.*(?:(?:FILEVERSION)|(?:FileVersion)|(?:PRODUCTVERSION)|(?:ProductVersion)).*?(?:\d*[.,]){3})(\d*)(.*)'

$tmp = Get-Content $file | Foreach {
        [regex]::replace($_, $regex, 
            {param($m) $m.Groups[1].Value + ([int]$m.Groups[2].Value + 1) + $m.Groups[3].Value})
    }

$tmp | Set-Content $file