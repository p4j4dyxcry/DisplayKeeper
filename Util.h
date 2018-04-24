#pragma once

//! 指定文字以上であれば省略記号で文字列を省略する
//! example ConvertToEllipsis(ABCDEFGHIJKLMNOPQ,10,'.') 
//! result  -> ABCDEFGH...
void ConvertToEllipsis(char* buffer, int count, char token = '.');

//! キーコードからキーの名前を取得する
//! example GetKeyNameEx(0x20,result,20)
//! result  -> "Space"
void GetKeyNameEx(char bitKeyCode, char* szBuf, int maxLength);

//! 名前からプロセスIDを取得する
DWORD GetProcessIdByName(const wchar_t *processName);