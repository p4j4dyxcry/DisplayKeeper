#pragma once

//! �w�蕶���ȏ�ł���Ώȗ��L���ŕ�������ȗ�����
//! example ConvertToEllipsis(ABCDEFGHIJKLMNOPQ,10,'.') 
//! result  -> ABCDEFGH...
void ConvertToEllipsis(char* buffer, int count, char token = '.');

//! �L�[�R�[�h����L�[�̖��O���擾����
//! example GetKeyNameEx(0x20,result,20)
//! result  -> "Space"
void GetKeyNameEx(char bitKeyCode, char* szBuf, int maxLength);

//! ���O����v���Z�XID���擾����
DWORD GetProcessIdByName(const wchar_t *processName);