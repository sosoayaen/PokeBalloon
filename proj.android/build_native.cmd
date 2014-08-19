@echo off
set APPNAME="PokeBalloon"

set DIR=%~dp0
set APP_ANDROID_ROOT=%DIR%
set APP_ROOT=%DIR%..

echo NDK_ROOT = %NDK_ROOT%
echo COCOS2DX_ROOT = %COCOS2DX_ROOT%
echo APP_ROOT = %APP_ROOT%
echo APP_ANDROID_ROOT = %APP_ANDROID_ROOT%

rem clean assets
if exist %APP_ANDROID_ROOT%assets (	rd /Q /S %APP_ANDROID_ROOT%assets )

mkdir %APP_ANDROID_ROOT%assets

rem copy resources
 
xcopy %APP_ROOT%\Resources\* %APP_ANDROID_ROOT%assets /c /d /s /y

del %APP_ANDROID_ROOT%assets\music\effect\*.mp3
del %APP_ANDROID_ROOT%assets\music\effect\*.wav
del %APP_ANDROID_ROOT%assets\music\effect\*.m4a

rem NDK_MODULE_PATH
set NDK_MODULE_PATH=%COCOS2DX_ROOT%;%COCOS2DX_ROOT%\cocos2dx\platform\third_party\android\prebuilt;

rem ndk-build
%NDK_ROOT%\ndk-build -j2
