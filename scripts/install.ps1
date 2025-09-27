param([string]$PS3IP)

$File = ".\patchwork.sprx"
$ftp = "ftp://$PS3IP/dev_hdd0/plugins/patchwork.sprx"

"ftp url: $ftp"

$webclient = New-Object System.Net.WebClient
$uri = New-Object System.Uri($ftp)

"Uploading $File..."

$webclient.UploadFile($uri, $File)