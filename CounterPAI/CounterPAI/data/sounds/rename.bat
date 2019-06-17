
setlocal EnableDelayedExpansion
set i=116
for %%a in (*.wav) do (
    set /a i+=1
    ren "%%a" "!i!.wav"
)
ren *.new *.wav