7-max 5.00 Sources
------------------

7-max is a program for accelerating your Windows 2000/XP system.
It increases the performance of some applications up to 10-20%.

7-max Copyright (C) 2003-2005 Igor Pavlov.


License Info
------------

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


unRAR license + GNU LGPL Notes


Registration
------------

You can support development of 7-Zip by registering.


Registration
------------

7-max is a free software. However, you can support 
development of 7-max by registering.
For information about how to register 7-max visit page

http://www.7-max.com/register.html


How to compile
--------------

To compile sources you will need Visual C++ 6.0 or a later version. 
Some files also require a new Platform SDK from microsoft.com:

http://www.microsoft.com/msdownload/platformsdk/sdkupdate/psdk-full.htm 
or 
http://www.microsoft.com/msdownload/platformsdk/sdkupdate/ 

If you are using MSVC, specify the SDK directories at the top of the 
"Include files" and "Library files" directory lists. These can be found 
under "Tools / Options / Directories".

The latest Platform SDK is not compatible with MSVC6. So you must use 
Windows Server 2003 PSDK (February 2003) with MSVC6.


To compile 7-Zip for AMD64 you need:
  Windows Server 2003 SP1 Platform SDK from microsoft.com


Description of 7-Zip sources package
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

DOC                Documentation
---
  7zFormat.txt   - 7z format description
  copying.txt    - GNU LGPL license
  unRarLicense.txt - License for unRAR part of source code
  history.txt    - Sources history
  Methods.txt    - Compression method IDs
  readme.txt     - Readme file
  lzma.txt       - LZMA SDK description
  7zip.nsi       - installer script for NSIS


Common            Common modules
Windows           Win32 wrappers

7zip
-------
  Common          Common modules for 7-zip

  Archive         7-Zip Archive Format Plugins 
  --------
    Common
    7z
    Arj
    BZip2
    Cab
    Cpio
    GZip
    Rar
    Rpm            
    Split
    Tar
    Zip

  Bundle          Modules that are bundles of other modules
  ------
    Alone         7za.exe: Standalone version of 7z
    SFXCon        7zCon.sfx: Console 7z SFX module
    SFXWin        7z.sfx: Windows 7z SFX module
    SFXSetup      7zS.sfx: Windows 7z SFX module for Installers
    Format7z      7za.dll: Standalone version of 7z.dll

  UI
  --
    Agent         Intermediary modules for FAR plugin and Explorer plugin
    Console       7z.exe Console version
    Explorer      Explorer plugin
    Resource      Resources
    Far           FAR plugin  
    Client7z      Test application for 7za.dll 

  Compress
  --------
    BZip2        BZip2 compressor
      Original   Download BZip2 compression sources from
                    http://sources.redhat.com/bzip2/index.html   
                 to that folder.
    Branch       Branch converter
    ByteSwap     Byte Swap converter
    Copy         Copy coder
    Deflate       
    Implode
    Arj
    LZMA
    PPMd          Dmitry Shkarin's PPMdH with small changes.
    LZ            Lempel - Ziv
      MT          Multi Thread Match finder
      BinTree     Match Finder based on Binary Tree
      Patricia    Match Finder based on Patricia algoritm
      HashChain   Match Finder based on Hash Chains

  Crypto          Crypto modules
  ------
    7zAES         Cipher for 7z
    AES           AES Cipher
    Rar20         Cipher for Rar 2.0
    RarAES        Cipher for Rar 3.0
    Zip           Cipher for Zip

  FileManager       File Manager


---
Igor Pavlov
http://www.7-zip.org


---
End of document

