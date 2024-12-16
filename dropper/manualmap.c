#include <Windows.h>
#include "misc.h"
// Zer0Mem0ry's MM

// 라이브러리 로더 함수
DWORD __stdcall LibraryLoader(LPVOID Memory)
{
    loaderdata* LoaderParams = (loaderdata*)Memory; // 메모리에서 로더 파라미터 구조체를 가져옴

    PIMAGE_BASE_RELOCATION pIBR = LoaderParams->BaseReloc; // 기본 재배치 정보 가져오기

    // 이미지의 새 기준 주소와 원래 주소의 차이를 계산
    DWORD delta = (DWORD)((LPBYTE)LoaderParams->ImageBase - LoaderParams->NtHeaders->OptionalHeader.ImageBase);

    // 재배치 프로세스 시작
    while (pIBR->VirtualAddress)
    {
        // 블록 크기가 IMAGE_BASE_RELOCATION 구조체 크기 이상인지 확인
        if (pIBR->SizeOfBlock >= sizeof(IMAGE_BASE_RELOCATION))
        {
            // 재배치 항목 수 계산
            int count = (pIBR->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
            PWORD list = (PWORD)(pIBR + 1); // 재배치 항목 리스트 포인터

            // 각 재배치 항목에 대해 처리
            for (int i = 0; i < count; i++)
            {
                if (list[i]) // 유효한 재배치 항목인지 확인
                {
                    // 재배치할 주소 계산
                    PDWORD ptr = (PDWORD)((LPBYTE)LoaderParams->ImageBase + (pIBR->VirtualAddress + (list[i] & 0xFFF)));
                    *ptr += delta; // 주소에 delta 추가
                }
            }
        }

        // 다음 재배치 블록으로 이동
        pIBR = (PIMAGE_BASE_RELOCATION)((LPBYTE)pIBR + pIBR->SizeOfBlock);
    }

    PIMAGE_IMPORT_DESCRIPTOR pIID = LoaderParams->ImportDirectory; // 임포트 디렉토리 가져오기

    // DLL 임포트 해결
    while (pIID->Characteristics)
    {
        // 원래의 첫 번째 스로우와 첫 번째 스로우를 가져옴
        PIMAGE_THUNK_DATA OrigFirstThunk = (PIMAGE_THUNK_DATA)((LPBYTE)LoaderParams->ImageBase + pIID->OriginalFirstThunk);
        PIMAGE_THUNK_DATA FirstThunk = (PIMAGE_THUNK_DATA)((LPBYTE)LoaderParams->ImageBase + pIID->FirstThunk);

        // DLL 로드
        HMODULE hModule = LoaderParams->fnLoadLibraryA((LPCSTR)LoaderParams->ImageBase + pIID->Name);

        if (!hModule) // DLL 로드 실패 시
            return FALSE;

        // 각 원래 스로우에 대해 처리
        while (OrigFirstThunk->u1.AddressOfData)
        {
            if (OrigFirstThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG) // 오디널을 통한 임포트
            {
                DWORD Function = (DWORD)LoaderParams->fnGetProcAddress(hModule,
                    (LPCSTR)(OrigFirstThunk->u1.Ordinal & 0xFFFF)); // 함수 주소 가져오기

                if (!Function) // 함수 주소를 가져오지 못한 경우
                    return FALSE;

                FirstThunk->u1.Function = Function; // 함수 주소 저장
            }
            else // 이름으로 임포트
            {
                PIMAGE_IMPORT_BY_NAME pIBN = (PIMAGE_IMPORT_BY_NAME)((LPBYTE)LoaderParams->ImageBase + OrigFirstThunk->u1.AddressOfData);
                DWORD Function = (DWORD)LoaderParams->fnGetProcAddress(hModule, (LPCSTR)pIBN->Name); // 함수 주소 가져오기
                if (!Function) // 함수 주소를 가져오지 못한 경우
                    return FALSE;

                FirstThunk->u1.Function = Function; // 함수 주소 저장
            }
            OrigFirstThunk++; // 다음 원래 스로우로 이동
            FirstThunk++; // 다음 첫 번째 스로우로 이동
        }
        pIID++; // 다음 임포트 디스크립터로 이동
    }

    // 엔트리 포인트가 존재하는 경우
    if (LoaderParams->NtHeaders->OptionalHeader.AddressOfEntryPoint)
    {
        dllmain EntryPoint = (dllmain)((LPBYTE)LoaderParams->ImageBase + LoaderParams->NtHeaders->OptionalHeader.AddressOfEntryPoint);
        return EntryPoint((HMODULE)LoaderParams->ImageBase, DLL_PROCESS_ATTACH, NULL); // 엔트리 포인트 호출
    }
    return TRUE; // 성공적으로 완료
}

// 스텁 함수 (아무 작업도 하지 않음)
DWORD __stdcall stub()
{
    return 0; // 0 반환
}
