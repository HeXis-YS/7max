// 7maxRegistry.cpp

#include "StdAfx.h"

#include "7maxRegistry.h"
#include "Windows/Registry.h"

using namespace NWindows;
using namespace NRegistry;

static const TCHAR *kCUBasePath = TEXT("Software\\7max");
static const TCHAR *kUseMTRR = TEXT("UseMTRR");
static const TCHAR *kAllocMin = TEXT("AllocMin");
static const TCHAR *kPhysLimit = TEXT("PhysLimit");
static const TCHAR *kMethod = TEXT("Method");

static const UInt32 kAllocMinDefault = (1 << 20);

static void SaveOption(const TCHAR *name, bool enabled)
{
  CKey key;
  key.Create(HKEY_CURRENT_USER, kCUBasePath);
  key.SetValue(name, enabled);
}

static bool ReadOption(const TCHAR *name, bool defaultValue)
{
  CKey key;
  if (key.Open(HKEY_CURRENT_USER, kCUBasePath, KEY_READ) == ERROR_SUCCESS)
  {
    bool enabled;
    if (key.QueryValue(name, enabled) == ERROR_SUCCESS)
      return enabled;
  }
  return defaultValue;
}

static void SaveValue(const TCHAR *name, UInt32 value)
{
  CKey key;
  key.Create(HKEY_CURRENT_USER, kCUBasePath);
  key.SetValue(name, value);
}

static UInt32 ReadValue(const TCHAR *name, UInt32 defaultValue)
{
  CKey key;
  if (key.Open(HKEY_CURRENT_USER, kCUBasePath, KEY_READ) == ERROR_SUCCESS)
  {
    UInt32 value;
    if (key.QueryValue(name, value) == ERROR_SUCCESS)
      return value;
  }
  return defaultValue;

}

void SaveUseMTRR(bool showDots) { SaveOption(kUseMTRR, showDots); }
bool ReadUseMTRR() { return ReadOption(kUseMTRR, true); }

UInt32 GetRecommendedAllocMin() {  return kAllocMinDefault; }
void SaveAllocMin(UInt32 size) { SaveValue(kAllocMin, size); }
UInt32 ReadAllocMin() { return ReadValue(kAllocMin, GetRecommendedAllocMin()); }

void SavePhysLimit(UInt32 size) { SaveValue(kPhysLimit, size); }
UInt32 ReadPhysLimit() { return ReadValue(kPhysLimit, 0); }

void SaveMetod(UInt32 method) { SaveValue(kMethod, method); }
UInt32 ReadMethod() { return ReadValue(kMethod, 1); }
