# Run Diff Gate on a patch file
param(
  [Parameter(Mandatory=$true)][string]$PatchFile
)

python .\agents\_shared\diff_gate.py $PatchFile
exit $LASTEXITCODE
