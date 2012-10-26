/* CE3 Plugin Manager - for licensing and copyright see license.txt */

#pragma once

#include <StdAfx.h>

namespace PluginManager
{
    /**
    * @internal
    * @brief Small Helper for verifying paths
    */
    static bool isAbsolute( const string sPath )
    {
        if ( sPath.length() < 1 )
        {
            return false;
        }

        if ( sPath[0] == *PATH_SEPERATOR )
        {
            return true;
        }

        if ( sPath[0] == '/' )
        {
            return true;
        }

        if ( sPath.length() < 2 )
        {
            return false;
        }

        if ( sPath[1] == ':' )
        {
            return true;
        }

        return false;
    }

    /**
    * @internal
    * @brief Small Helper for verifying if path is not an empty string
    */
    static bool isEmpty( const char* sPath )
    {
        if ( !sPath || !sPath[0] )
        {
            return true;
        }

        return false;
    }

    /**
    * @internal
    * @brief Small Helper for verifying if path is dot special path
    */
    static bool isDot( const string sPath )
    {
        if ( sPath == "." || sPath == ".." )
        {
            return true;
        }

        return false;
    }

    /**
    * @internal
    * @brief Small Helper for verifying terminating slash
    */
    static string pathWithSeperator( const string sPath )
    {
        string sRet = sPath; // fallback

        if ( sPath.length() < 1 )
        {
            return sRet;
        }

        if ( sPath.Right( 1 ).at( 0 ) != PATH_SEPERATOR[0] )
        {
            sRet += PATH_SEPERATOR;
        }

        return sRet;
    }

    /**
    * @internal
    * @brief Small Helper for extracting directory from a path
    */
    static string pathBaseName( const string sPath )
    {
        string sRet = sPath; // fallback

        size_t nEndPos = sPath.find_last_of( PATH_SEPERATOR );

        if ( nEndPos != string::npos && nEndPos > 0 )
        {
            sRet = sPath.Left( nEndPos );
        }

        return sRet;
    }

    /**
    * @internal
    * @brief Small Helper for extracting file from a path
    */
    static string pathFileName( const string sPath )
    {
        string sRet = sPath; // fallback

        size_t nStartPos = sPath.find_last_of( PATH_SEPERATOR );

        if ( nStartPos != string::npos && nStartPos + 1 < sPath.length() )
        {
            sRet = sPath.Mid( nStartPos + 1 );
        }

        return sRet;
    }

    /**
    * @internal
    * @brief Convert Active Codepage to UTF-8
    * @param sSource ACP String
    * @return UTF-8 String
    */
    string ACP2UTF8( const string sSource )
    {
        const char* pBuffer = sSource.c_str();
        int nLength = sSource.length();

        // Convert file from ACP to UTF-8 via the Windows UNICODE (AKA UCS-2)
        int nWideLength = MultiByteToWideChar( CP_ACP, 0, pBuffer, nLength, NULL, 0 );
        wchar_t* pWideBuffer = new wchar_t[nWideLength];
        MultiByteToWideChar( CP_ACP, 0, pBuffer, nLength, pWideBuffer, nWideLength );

        int nUtf8Length = WideCharToMultiByte( CP_UTF8, 0, pWideBuffer, nWideLength, NULL, 0, NULL, NULL );
        char* pUtf8Buffer = new char[nUtf8Length];
        WideCharToMultiByte( CP_UTF8, 0, pWideBuffer, nWideLength, pUtf8Buffer, nUtf8Length, NULL, NULL );

        string utf8 ( pUtf8Buffer, nUtf8Length );
        delete [] pWideBuffer;
        delete [] pUtf8Buffer;

        return utf8;
    }

    /**
    * @internal
    * @brief Convert UTF-8 to Active Codepage
    * @param sSource UTF-8 String
    * @return ACP String
    */
    string UTF82ACP( const string sSource )
    {
        const char* pBuffer = sSource.c_str();
        int nLength = sSource.length();

        // Convert file from UTF-8 to ACP via the Windows UNICODE (AKA UCS-2)
        int nWideLength = MultiByteToWideChar( CP_UTF8, 0, pBuffer, nLength, NULL, 0 );
        wchar_t* pWideBuffer = new wchar_t[nWideLength];
        MultiByteToWideChar( CP_UTF8, 0, pBuffer, nLength, pWideBuffer, nWideLength );

        int nUtf8Length = WideCharToMultiByte( CP_ACP, 0, pWideBuffer, nWideLength, NULL, 0, NULL, NULL );
        char* pUtf8Buffer = new char[nUtf8Length];
        WideCharToMultiByte( CP_ACP, 0, pWideBuffer, nWideLength, pUtf8Buffer, nUtf8Length, NULL, NULL );

        string utf8 ( pUtf8Buffer, nUtf8Length );
        delete [] pWideBuffer;
        delete [] pUtf8Buffer;

        return utf8;
    }

}