// ContextMenu.h

#ifndef __CONTEXTMENU_H
#define __CONTEXTMENU_H

// {23170F69-40C2-278A-1000-000100020000}
DEFINE_GUID(CLSID_C7maxContextMenu, 
0x23170F69, 0x40C2, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x00);

#include "Common/String.h"
#include "MyCom2.h"

class C7maxContextMenu: 
  public IContextMenu,
  public IShellExtInit,
  public CMyUnknownImp
{
public:
MY_UNKNOWN_IMP2_MT(IContextMenu, IShellExtInit)

  ///////////////////////////////
  // IShellExtInit

  STDMETHOD(Initialize)(LPCITEMIDLIST pidlFolder, 
      LPDATAOBJECT dataObject, HKEY hkeyProgID);

  /////////////////////////////
  // IContextMenu
  
  STDMETHOD(QueryContextMenu)(HMENU hmenu, UINT indexMenu,
      UINT idCmdFirst, UINT idCmdLast, UINT uFlags);
  STDMETHOD(InvokeCommand)(LPCMINVOKECOMMANDINFO lpici);
  STDMETHOD(GetCommandString)(UINT_PTR idCmd, UINT uType, UINT *pwReserved,
      LPSTR pszName, UINT cchMax);

private:
  UStringVector _fileNames;
  HRESULT GetFileNames(LPDATAOBJECT dataObject, CSysStringVector &fileNames);
};

#endif
