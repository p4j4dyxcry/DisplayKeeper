#include "stdafx.h"
#include <tlhelp32.h>
//! �����񂪎w�蕶���ȏ�ł���Ώȗ��L��������
//! example �\�����10�ɗ}����ꍇ hogehoge
void ConvertToEllipsis(char* buffer, int count , char token )
{
    // ���Ȃ����镶����ɑ΂��Ă͖���
    if (count < 5)
        return ;

    //! ����̕������ȓ��Ȃ�L���Ȃ̂ŏȗ��L���͓���Ȃ�
    if (buffer[count - 4] == '\0')return;
    if (buffer[count - 3] == '\0')return;
    if (buffer[count - 2] == '\0')return;
    if (buffer[count - 1] == '\0')return;

    //! �ȗ��L���Œu��������
    buffer[count - 4] =
    buffer[count - 3] =
    buffer[count - 2] = token ;
    
    buffer[count - 1] = '\0';
}

//! �L�[�R�[�h����L�[�̖��O���擾����
void GetKeyNameEx(char bitKeyCode, char* szBuf, int maxLength)
{
    UINT nScanCode = MapVirtualKey(bitKeyCode, 0);

    
    //! Win32API�ł͑Ή����Ă��Ȃ��̂œ���Ή�
    if(bitKeyCode == VK_PAUSE)
    {
        strcpy_s(szBuf, maxLength,"Pause");
        return;
    }

    //! �Ή����Ă��Ȃ��Ǝv����L�[�͂����Ő����ǉ�����
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

//! ���O����v���Z�XID���擾����
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
