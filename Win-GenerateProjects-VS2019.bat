call git submodule sync
call git submodule init
call git submodule update

call Scripts\premake\premake5.exe vs2019
IF %ERRORLEVEL% NEQ 0 (
  PAUSE
)
