#pragma once


#include <Windows.h>

#define DEFAULT_INT_VAL -1
#define READ_STRING_FAIL -1 
#define DEFAULT_STRING L"STRING_READ_FAIL"


class CINIFileReader
{
private:

    TCHAR m_szIniFile[ MAX_PATH ];

public:

    CINIFileReader( LPCTSTR pszIniFile=NULL );

    void SetIniFile( LPCTSTR pszIniFile );

    //get an integer value from the specified location in the .ini file
    //failed reads will automatically return -1 
    int ReadInt(LPCTSTR section, LPCTSTR key, int DefaultValue=DEFAULT_INT_VAL);
    float ReadFloat( LPCTSTR section, LPCTSTR key, float DefaultValue=0.0f);
    float ReadBrace4Float( LPCTSTR section, LPCTSTR key, float *fArray, float DefaultValue=0.0f);
    int GetIntErrorValue(void) {return DEFAULT_INT_VAL;}
    int ReadString(LPCTSTR section, LPCTSTR key, LPTSTR pBuf, int iSizeBuf, LPCTSTR DefaultValue=DEFAULT_STRING);
    int GetStringErrorValue(void) {return READ_STRING_FAIL;}
};
