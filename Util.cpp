#include "stdafx.h"
#include <tlhelp32.h>
//! 文字列が指定文字以上であれば省略記号を入れる
//! example 表示列を10に抑える場合 hogehoge
void ConvertToEllipsis(char* buffer, int count , char token )
{
    // 少なすぎる文字列に対しては無効
    if (count < 5)
        return ;

    //! 既定の文字数以内なら有効なので省略記号は入れない
    if (buffer[count - 4] == '\0')return;
    if (buffer[count - 3] == '\0')return;
    if (buffer[count - 2] == '\0')return;
    if (buffer[count - 1] == '\0')return;

    //! 省略記号で置き換える
    buffer[count - 4] =
    buffer[count - 3] =
    buffer[count - 2] = token ;
    
    buffer[count - 1] = '\0';
}

//! キーコードからキーの名前を取得する
void GetKeyNameEx(char bitKeyCode, char* szBuf, int maxLength)
{
    UINT nScanCode = MapVirtualKey(bitKeyCode, 0);

    
    //! Win32APIでは対応していないので特殊対応
    if(bitKeyCode == VK_PAUSE)
    {
        strcpy_s(szBuf, maxLength,"Pause");
        return;
    }

    //! 対応していないと思われるキーはここで随時追加する
    ///
    ///

    if (nScanCode) {
        switch (bitKeyCode) {
        case VK_LEFT:
        case VK_UP:
        case VK_RIGHT:
        case VK_DOWN:
        case VK_PRIOR:
        case VK_NEXT:
        case VK_END:
        case VK_HOME:
        case VK_INSERT:
        case VK_DELETE:
        case VK_DIVIDE:
        case VK_NUMLOCK:
            nScanCode |= 0x100;
            break;
        }        
        GetKeyNameTextA(nScanCode << 16, szBuf, maxLength);
    }
}

//! 名前からプロセスIDを取得する
DWORD GetProcessIdByName(const wchar_t *processName)
{
    auto entry = PROCESSENTRY32{ sizeof(PROCESSENTRY32) };

    auto hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (Process32First(hSnapshot, &entry))
    {
        do
        {
            if (wcsstr(processName, entry.szExeFile))
            {
                CloseHandle(hSnapshot);
                return entry.th32ProcessID;
            }
        } while (Process32Next(hSnapshot, &entry));
    }

    CloseHandle(hSnapshot);
    return -1;
}
