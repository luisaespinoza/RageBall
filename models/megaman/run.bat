@echo off
for %%f in (*.pcx) do (
    echo Converting %%f ...
    magick "%%f" "%%~nf.jpg"
)
echo Done!
pause
