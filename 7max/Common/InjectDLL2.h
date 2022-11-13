// InjectDLL2.h

#ifndef __INJECTDLL2_H
#define __INJECTDLL2_H

bool InjectViaRemoteThread(HANDLE process, const wchar_t *dllPath, FARPROC loadLibraryWAddress);

#endif
