// WindowActivetor.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "Util.h"
#include <list>

//! コマンドを実行するマクロ
#define SEND_COMMAND(command,...) \
{\
     /* 区切り行 */ \
    const char* SEPARTOR = "----------------------------------------------------\n";\
    /*sセパレータ表示*/\
    std::cout << SEPARTOR;\
    /*コマンド実行ログの出力*/\
    std::cout << __VA_ARGS__ << std::endl;\
    /*コマンド実行*/\
    command();\
    /*コマンドは0.25秒に1回しか送信できない*/\
    Sleep(250);\
}

void DumpLastError()
{
    char messageBuffer[512];
    DWORD lastError = GetLastError();
    if (lastError == 0)
        return;
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        GetLastError(),
        LANG_USER_DEFAULT,
        messageBuffer,
        512,
        NULL);
    std::cout << "エラー"<< std::endl << messageBuffer << std::endl;
}

int main(int argc , char* argv[])
{  
    const char* AppName = "DisplayKeeper1.01";
    if (GetProcessIdByName(L"DisplayKeeper.exe") != GetCurrentProcessId())
    {
        MessageBoxA(nullptr,"多重起動はできません。","エラー",MB_OK);
        return -1;
    }
    //! アプリ　自体の有効 / 無効
    const char EnableKeyCode = VK_F1;
    bool enableApp = true;

    //! 最前面の有効 / 無効
    const char SwitchKeyCode = VK_F2;

    //! 前回の状態を保存しておくための辞書
    std::map<HWND, bool> top_most_dictionary;

    SetConsoleTitleA(AppName);
    Sleep(250);
    const HWND AppHandle = FindWindowA(nullptr,AppName);


    //! Commands---------------------------------------------------------------------

    const auto ShowHelpCommand = [&]
    {
        char keyCodeString[128];

        GetKeyNameEx(SwitchKeyCode, keyCodeString, 128);
        std::cout << "最前面固定する        :" << keyCodeString << "キー" << std::endl;

        GetKeyNameEx(EnableKeyCode, keyCodeString, 128);
        std::cout << "このアプリを有効/無効 :" << keyCodeString << "キー" << std::endl;

        GetKeyNameEx(VK_ESCAPE, keyCodeString, 128);
        std::cout << "終了                  :" << keyCodeString << "キー" << std::endl;
    };

    const auto ExitCommand = ([&]
    {
        for (const auto it : top_most_dictionary)
        {
            if (it.first)
            {
                if (SetWindowPos(it.first, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE))
                    DumpLastError();
                top_most_dictionary[it.first] = false;
            }
        }
    });

    const auto ChangeEnableCommand = ([&]
    {
        enableApp ^= true;
        if (enableApp == false)
        {
            std::list<HWND> removeItems;
            for (const auto it : top_most_dictionary)
            {
                if (it.second)
                {
                    if(SetWindowPos(it.first, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE) != 0)
                    {
                        DumpLastError();
                        removeItems.push_back(it.first);
                        continue;
                    }
                    top_most_dictionary[it.first] = false;

                    char windowTitle[256];
                    if( GetWindowTextA(it.first, windowTitle, 256))
                    {
                        std::cout << "ウィンドウ :" << windowTitle << std::endl;
                        std::cout << "最前面設定を無効にしました。" << std::endl;
                    }
                    else
                    {
                        std::cout << "エラー 無効なウィンドウハンドルを検知" << std::endl;
                        removeItems.push_back(it.first);
                    }
                }
            }

            for (auto it: removeItems)
                top_most_dictionary.erase(it);
        }
    });

    const auto KeepActiveWindowCommand = ([&]
    {
        auto activeHwnd = GetForegroundWindow();

        if(activeHwnd == nullptr)
        {
            std::cout << "エラー アクティブなWindowの取得に失敗" << std::endl ;
            return;
        }

        char windowTitle[256];
        if( GetWindowTextA(activeHwnd, windowTitle, 256) == 0)
        {
            std::cout << "エラー 無効なウィンドウハンドルを検知" << std::endl;
            top_most_dictionary.erase(activeHwnd);
            return;
        }
        if(strcmp(windowTitle, AppName) == 0)
        {
            char keyCodeString[128];
            GetKeyNameEx(SwitchKeyCode, keyCodeString, 128);
            std::cout << "失敗" << std::endl;
            std::cout << "最全面で固定したいウィンドウを選択して" << keyCodeString << "キーを押してください。" << std::endl;
            return;
        }

        bool isTopMost = true;
        if (top_most_dictionary.find(activeHwnd) == top_most_dictionary.end())
            top_most_dictionary.insert(std::make_pair(activeHwnd, isTopMost));
        else
        {
            // xor switch
            isTopMost = top_most_dictionary.find(activeHwnd)->second ^= true;
            top_most_dictionary.insert(std::make_pair(activeHwnd, isTopMost));
        }

        try
        {
            if(SetWindowPos(activeHwnd, isTopMost ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE) != 0)
                DumpLastError();
        }
        catch (std::exception& e)
        {
            //! SetWindowPos()が失敗した場合
            std::cout << "■ Error!!" << std::endl << e.what();
        }

        //! 60文字以上は2行になるのが鬱陶しいので省略する。
        ConvertToEllipsis(windowTitle, 60);

        std::cout << "ウィンドウ :" << windowTitle << std::endl;
        std::cout << "最前面設定 :" << (isTopMost ? "有効" : "無効") << std::endl;
    });

    //-------------------------------------------------------------------------------

    ShowHelpCommand();
    while(true)
    {
        //! アプリケーション終了
        if (GetAsyncKeyState(VK_ESCAPE) && GetForegroundWindow() == AppHandle)
        {
            SEND_COMMAND(ExitCommand, "アプリケーションを終了");
            return 0;
        }
        //! DisplayKeeperの有効 / 無効切替
        if (GetAsyncKeyState(EnableKeyCode) && GetForegroundWindow() == AppHandle)
        {
            SEND_COMMAND(ChangeEnableCommand, "DisplayKeeperを" << (!enableApp ? "有効" : "無効") << "に設定");
        }

        //! アクティブなウィンドウを常に最前面表示にする
        else if( GetAsyncKeyState(SwitchKeyCode) && enableApp)
        {
            SEND_COMMAND(KeepActiveWindowCommand, "DisplayKeepを実行");
        }
    }
    return 0;
}

