

#include <tchar.h>
#include "INIFileReader.h"


CINIFileReader::CINIFileReader(
    LPCTSTR pszIniFile
    )
{
    if ( pszIniFile != NULL )
    {
        _tcscpy_s( m_szIniFile, _countof (m_szIniFile), pszIniFile );
    }
    else
    {
        m_szIniFile[ 0 ] = '\0';
    }
}


void
CINIFileReader::SetIniFile(
    LPCTSTR pszIniFile
    )
{
    _tcscpy_s( m_szIniFile, _countof (m_szIniFile), pszIniFile );
}


int CINIFileReader::ReadInt( LPCTSTR section, LPCTSTR key, int DefaultValue )
{
    int retval = 0;
    
    retval = GetPrivateProfileInt(
        section,
        key,
        DefaultValue,
        m_szIniFile);

    return retval;
}

float CINIFileReader::ReadFloat( LPCTSTR section, LPCTSTR key, float DefaultValue )
{
    TCHAR szBuf[ 50 ] = _T( "" );

    GetPrivateProfileString(
        section,
        key,
        _T( "" ),
        szBuf,
        50,
        m_szIniFile);

    float retVal = DefaultValue;
    if ( szBuf[ 0 ] != '\0' )
    {
        _stscanf_s( szBuf, _T( "%f" ), &retVal );
    }
    return retVal;
}

float CINIFileReader::ReadBrace4Float( LPCTSTR section, LPCTSTR key, float *fArray, float DefaultValue )
{
    TCHAR szBuf[ 50 ] = _T( "" );

    GetPrivateProfileString(
        section,
        key,
        _T( "" ),
        szBuf,
        50,
        m_szIniFile);

    float retVal = DefaultValue;
    if ( szBuf[ 0 ] != '\0' )
    {
        _stscanf_s( szBuf, _T( "%f" ) _T( "%f" ) _T( "%f" ) _T( "%f" ), &fArray[0], &fArray[1], &fArray[2], &fArray[3] );
    }
    return retVal;
}

int CINIFileReader::ReadString( LPCTSTR section, LPCTSTR key, LPTSTR pBuf, int iSizeBuf, LPCTSTR DefaultValue )
{
    int retval = 0;
    retval = GetPrivateProfileString(
        section,
        key,
        DefaultValue,
        pBuf,
        iSizeBuf,
        m_szIniFile);

    //fail conditions
    if ((retval == (iSizeBuf - 1)) || (retval == (iSizeBuf - 2)))
    {
        retval = READ_STRING_FAIL;
    }
    else    
    {
        retval = 0;
    }

    return retval;
}