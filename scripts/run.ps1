
param([string]$PS3IP)

Invoke-WebRequest "http://$PS3IP/play.ps3"