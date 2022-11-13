// x86CPUID.h

#ifndef __X86CPUID_H
#define __X86CPUID_H

namespace NCPUID {
  
bool IsMTRRSupported();
bool IsPSESupported();
bool IsPSE36Supported();

}

#endif
