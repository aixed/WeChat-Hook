:: 原微信更新屏蔽代码
echo Y|cacls "%USERPROFILE%\AppData\Roaming\Tencent\xwechat\update\update.data" /T /P %USERNAME%:N >nul 2>nul
echo 步骤2已完成。
rd /s /q "%USERPROFILE%\AppData\Roaming\Tencent\xwechat\update\patch" >nul 2>nul
md "%USERPROFILE%\AppData\Roaming\Tencent\xwechat\update\patch" >nul 2>nul
echo Y|cacls "%USERPROFILE%\AppData\Roaming\Tencent\xwechat\update\patch" /T /P %USERNAME%:N >nul 2>nul
echo 步骤3已完成。

:: 退出脚本
echo 所有设置已完成，正在自动退出。
timeout /nobreak /t 2 >nul 2>nul
exit