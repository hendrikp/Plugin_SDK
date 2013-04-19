/* CE3 Plugin Manager - for licensing and copyright see license.txt */

/**
* @file
* Utility functions that can be used directly by anyone
*/

#pragma once

#include <StdAfx.h>

#if defined(WIN32) || defined(WIN64)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
// get rid of (really) annoying MS defines
#undef min
#undef max
#undef WIN32_LEAN_AND_MEAN
#endif

#include <sstream>

#ifndef PATH_SEPERATOR
#define PATH_SEPERATOR "\\"
#endif

#ifndef SAFESTR
#define SAFESTR(x) (((const char*)x)?((const char*)x):"") //!< Helper to avoid NULL str problems
#endif

#undef STRING_TRUE
#define STRING_TRUE "true" //!< true value
#undef STRING_FALSE
#define STRING_FALSE "false" //!< false value

#ifndef BOOLSTR
#define BOOLSTR(x) (x?STRING_TRUE:STRING_FALSE) //!< Helper to convert boolean values
#endif

namespace PluginManager
{
    /**
    * @internal
    * @brief Small Helper for verifying if a path is absolute
    * @param sPath path to verify
    * @return true when absolute
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
    * @param sPath path to verify
    * @return true when empty
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
    * @param sPath path to check
    * @return true when special
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
    * @brief Small Helper for adding/verifying terminating slash
    * @param sPath path
    * @return path with terminating slash
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
    * @param sPath path
    * @return directory part of the path
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
    * @param sPath path
    * @return filename part of the path
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
    static string ACP2UTF8( const string sSource )
    {
        const char* pBuffer = sSource.c_str();
        int nLength = sSource.length();

        // Convert string from ACP to UTF-8 via the Windows UNICODE (AKA UCS-2)
        int nWideLength = MultiByteToWideChar( CP_ACP, 0, pBuffer, nLength, NULL, 0 );
        wchar_t* pWideBuffer = new wchar_t[nWideLength];
        MultiByteToWideChar( CP_ACP, 0, pBuffer, nLength, pWideBuffer, nWideLength );

        int nUtf8Length = WideCharToMultiByte( CP_UTF8, 0, pWideBuffer, nWideLength, NULL, 0, NULL, NULL );
        char* pUtf8Buffer = new char[nUtf8Length];
        WideCharToMultiByte( CP_UTF8, 0, pWideBuffer, nWideLength, pUtf8Buffer, nUtf8Length, NULL, NULL );

        string sUTF8 ( pUtf8Buffer, nUtf8Length );
        delete [] pWideBuffer;
        delete [] pUtf8Buffer;

        return sUTF8;
    }

    /**
    * @internal
    * @brief Convert UTF-8 to Active Codepage
    * @param sSource UTF-8 String
    * @return ACP String
    */
    static string UTF82ACP( const string sSource )
    {
        const char* pBuffer = sSource.c_str();
        int nLength = sSource.length();

        // Convert string from UTF-8 to ACP via the Windows UNICODE (AKA UCS-2)
        int nWideLength = MultiByteToWideChar( CP_UTF8, 0, pBuffer, nLength, NULL, 0 );
        wchar_t* pWideBuffer = new wchar_t[nWideLength];
        MultiByteToWideChar( CP_UTF8, 0, pBuffer, nLength, pWideBuffer, nWideLength );

        int nUtf8Length = WideCharToMultiByte( CP_ACP, 0, pWideBuffer, nWideLength, NULL, 0, NULL, NULL );
        char* pUtf8Buffer = new char[nUtf8Length];
        WideCharToMultiByte( CP_ACP, 0, pWideBuffer, nWideLength, pUtf8Buffer, nUtf8Length, NULL, NULL );

        string sACP( pUtf8Buffer, nUtf8Length );
        delete [] pWideBuffer;
        delete [] pUtf8Buffer;

        return sACP;
    }

    /**
    * @internal
    * @brief Convert UCS-2 to UTF-8
    * @param sSource UCS-2 String
    * @return UTF-8 String
    */
    static string UCS22UTF8( const wstring sSource )
    {
        const wchar_t* pBuffer = sSource.c_str();
        int nLength = sSource.length();

        // Convert string from Windows UNICODE (AKA UCS-2) to UTF-8
        int nUtf8Length = WideCharToMultiByte( CP_UTF8, 0, pBuffer, nLength, NULL, 0, NULL, NULL );
        char* pUtf8Buffer = new char[nUtf8Length];
        WideCharToMultiByte( CP_UTF8, 0, pBuffer, nLength, pUtf8Buffer, nUtf8Length, NULL, NULL );

        string sUTF8( pUtf8Buffer, nUtf8Length );
        delete [] pUtf8Buffer;

        return sUTF8;
    }

    /**
    * @internal
    * @brief Convert UTF-8 to UCS-2
    * @param sSource UTF-8 String
    * @return UCS-2 String
    */
    static wstring UTF82UCS2( const string sSource )
    {
        const char* pBuffer = sSource.c_str();
        int nLength = sSource.length();

        // Convert string from UTF-8 to Windows UNICODE (AKA UCS-2)
        int nWideLength = MultiByteToWideChar( CP_UTF8, 0, pBuffer, nLength, NULL, 0 );
        wchar_t* pWideBuffer = new wchar_t[nWideLength];
        MultiByteToWideChar( CP_UTF8, 0, pBuffer, nLength, pWideBuffer, nWideLength );

        wstring sUCS2 ( pWideBuffer, nWideLength );
        delete [] pWideBuffer;

        return sUCS2;
    }

    /**
    * @internal
    * @brief delayed function type
    * @param pData data for the delayed call
    * This is the delayed call
    */
    typedef void ( *tDelayedCall )( void* );

    /**
    * @internal
    * @brief delay call trigger type
    * Delay Trigger type functions can be used to monitor dynamic conditions
    * @param pDataTrigger data for the delay trigger
    * On true this executes the delayed call
    */
    typedef bool ( *tDelayedCallTrigger )( void* );

    /**
    * @internal
    * @brief class to delay function calls
    */
    class CallDelay
    {
        protected:
            tDelayedCall m_pFunc; //!< function pointer delayed function
            tDelayedCall m_pFuncCleanup; //!< cleanup function

            void* m_pData; //!< data for delayed function
            eDelayType m_eDT; //!< type of delay
            float m_fLeft; //!< delay amount left

            tDelayedCallTrigger m_pFuncTrigger; //!< function pointer delay trigger function
            tDelayedCall m_pFuncTriggerCleanup; //!< trigger cleanup function
            void* m_pDataTrigger; //!< data for delay trigger function

        private:
            CallDelay() {};
            //CallDelay( CallDelay& copy ) {};

        public:

            /**
            * @brief ~CallDelay destructor
            * Calls Cleanup functions if present
            */
            ~CallDelay()
            {
                if ( m_pFuncCleanup )
                {
                    m_pFuncCleanup( m_pData );
                    m_pData = NULL;
                }

                if ( m_pFuncTriggerCleanup )
                {
                    m_pFuncTriggerCleanup( m_pDataTrigger );
                    m_pDataTrigger = NULL;
                }
            };

            /**
            * @internal
            * @brief CallDelay constructor
            * The Delayed function and Trigger function can be used separated or together
            * @param pFunc function pointer of delayed function
            * @param pFuncCleanup function pointer that cleans up the data when the delayed call is canceled or finished
            * @param pData Data for delayed function
            * @param fDelay delay amount
            * @param eType delay type
            * @param pFuncTrigger function pointer delay trigger function
            * @param pFuncTriggerCleanup function pointer that cleans up the data when the delayed call is canceled or finished
            * @param pDataTrigger Data for delay trigger function
            */
            CallDelay( tDelayedCall pFunc = NULL, tDelayedCall pFuncCleanup = NULL, void* pData = NULL, float fDelay = 1.0f, eDelayType eType = eDT_Default, tDelayedCallTrigger pFuncTrigger = NULL, tDelayedCall pFuncTriggerCleanup = NULL, void* pDataTrigger = NULL )
            {
                // Standard
                m_pFunc = pFunc;
                m_pFuncCleanup = pFuncCleanup;
                m_pData = pData;
                m_eDT = eType;
                m_fLeft = fDelay;

                // Trigger type
                m_pFuncTrigger = pFuncTrigger;
                m_pDataTrigger = pDataTrigger;
                m_pFuncTriggerCleanup = pFuncTriggerCleanup;
            }

            /**
            * @internal
            * @brief Advance the delayed call
            * @param fDeltaTime time passed in seconds
            * @return is finished
            */
            bool Advance( float fDeltaTime )
            {
                if ( m_eDT == eDT_Frames )
                {
                    --m_fLeft;
                }

                else if ( m_eDT == eDT_Seconds )
                {
                    m_fLeft -= fDeltaTime;
                }

                if ( m_fLeft <= 0 || m_eDT == eDT_None )
                {

                    if ( m_pFunc )
                    {
                        m_pFunc( m_pData );
                    }

                    return true;
                }

                if ( m_eDT == eDT_Trigger )
                {
                    bool bRet = true;

                    if ( m_pFuncTrigger )
                    {
                        bRet = m_pFuncTrigger( m_pData );

                        if ( bRet && m_pFunc )
                        {
                            m_pFunc( m_pData );
                        }
                    }

                    return bRet;
                }

                return false;
            }
    };

    /**
    * @internal
    * @brief class to delay function calls
    */
    class CallDelayQueue
    {
        private:
            std::map<string, std::list<std::shared_ptr<CallDelay>>> m_queue;

        public:
            /*
            * @internal
            * @brief Delay a function
            * @param sFilter custom filter for remove control
            * @param pFunc function pointer of delayed function
            * @param pFuncCleanup function pointer that cleans up the data when the delayed call is canceled or finished
            * @param pData Data for delayed function
            * @param fDelay delay amount
            * @param eType delay type
            * @param pFuncTrigger function pointer delay trigger function
            * @param pFuncTriggerCleanup function pointer that cleans up the data when the delayed call is canceled or finished
            * @param pDataTrigger Data for delay trigger function
            */
            void DelayFunction( const char* sFilter = NULL, tDelayedCall pFunc = NULL, tDelayedCall pFuncCleanup = NULL, void* pData = NULL, float fDelay = 1.0f, eDelayType eType = eDT_Default, tDelayedCallTrigger pFuncTrigger = NULL, tDelayedCall pFuncTriggerCleanup = NULL, void* pDataTrigger = NULL )
            {
                m_queue[ SAFESTR( sFilter ) ].push_back( std::shared_ptr<CallDelay>( new CallDelay( pFunc, pFuncCleanup, pData, fDelay, eType, pFuncTrigger, pFuncTriggerCleanup, pDataTrigger ) ) );
            }

            /**
            * @internal
            * @brief Advance all delayed calls in this queue
            * @param fDeltaTime time passed in seconds
            */
            void AdvanceAll( float fDeltaTime )
            {
                bool bFinished = false;

                // for each filter
                for ( auto mIter = m_queue.begin(); mIter != m_queue.end(); ++mIter )
                {
                    auto& lQueue = mIter->second;

                    // for each delayed call of this filter
                    for ( auto qIter = lQueue.begin(); qIter != lQueue.end(); ++qIter )
                    {
                        // Do the processing
                        bFinished = ( *qIter )->Advance( fDeltaTime );

                        // Execution of this function is finished
                        if ( bFinished )
                        {
                            // Clean it up and fix iterator
                            qIter = lQueue.erase( qIter );

                            if ( qIter == lQueue.end() )
                            {
                                break;
                            }
                        }
                    }

                    // if the filter is empty, clean it up
                    if ( !lQueue.size() )
                    {
                        mIter = m_queue.erase( mIter );

                        if ( mIter == m_queue.end() )
                        {
                            break;
                        }

                        break;
                    }
                }
            }

            /**
            * @brief Cancels registered delayed calls
            * @param sFilter only delete these delayed calls (if NULL then everything is deleted)
            */
            void Cancel( const char* sFilter = NULL )
            {
                if ( !sFilter )
                {
                    m_queue.clear();
                }

                else
                {
                    m_queue.erase( sFilter );
                }
            }
    };

    /**
    * @internal
    * @brief Delayed Console Commands
    * @param sCommand to execute
    */
    static void _DelayedConsoleCommand( void* sCommand )
    {
        if ( sCommand )
        {
            if ( gEnv && gEnv->pConsole )
            {
                gEnv->pConsole->ExecuteString( ( const char* )sCommand );
            }
        }
    }

    /**
    * @internal
    * @brief Delayed Console Commands cleanup
    * @param sCommand to delete[]
    */
    static void _CleanupDelayedConsoleCommand( void* sCommand )
    {
        if ( sCommand )
        {
            delete [] ( const char* )( sCommand );
        }
    }

    /**
    * @internal
    * @brief Delays a console command
    * Since the built in defer didn't work as expected
    * @param qDelayer Delay Queue that handles this command
    * @param sCommand the console command
    * @param sFilter filter for the console command (can be used to cancel the command) default is "consolecommand"
    * @param fDelay delay amount
    * @param eType delay type
    */
    static void DelayConsoleCommand( CallDelayQueue& qDelayer, string sCommand, const char* sFilter = NULL, float fDelay = 1, eDelayType eType = eDT_Default, tDelayedCallTrigger pFuncTrigger = NULL, tDelayedCall pFuncTriggerCleanup = NULL, void* pDataTrigger = NULL )
    {
        if ( sCommand.length() > 0 )
        {
            // Prepare data
            size_t nLen = sCommand.length() + 1;
            char* sCmd = new char[nLen];
            strncpy( sCmd, sCommand.c_str(), nLen );
            sCmd[nLen - 1] = 0;

            const char* sFilterC = ( sFilter && sFilter[0] ) ? sFilter : "consolecommand";

            qDelayer.DelayFunction( sFilter, &_DelayedConsoleCommand, &_CleanupDelayedConsoleCommand, sCmd, fDelay, eType, pFuncTrigger, pFuncTriggerCleanup, pDataTrigger );
        }
    }

    /**
    * @internal
    * @brief split string into string array
    * @param sSource string to split
    * @param sDelimiter delimiter to use
    * @param[out] vTarget output array
    * @return success
    *  @arg else element count
    */
    static int stringExplode( string sSource, string sDelimiter, std::vector<string>& vTarget )
    {
        size_t nStart = 0;
        int nLen = 0;
        int i = 0;
        vTarget.clear();

        nLen = sDelimiter.length();

        while ( ( sSource = sSource.find( sDelimiter ) ) != string::npos )
        {
            vTarget.push_back( sSource.substr( 0, nStart ) );
            sSource.erase( 0, nStart + nLen );
            i++;
        }

        vTarget.push_back( sSource );

        return i;
    };

    /**
    * @internal
    * @brief small helper for getting last OS error text
    * @return error text
    */
    static string getOSError()
    {
        string sReturn = "";

        PVOID   sMessage    = NULL;
        DWORD   nCode       = GetLastError();

        if ( nCode )
        {
            FormatMessage(
                FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER,
                NULL,
                nCode,
                MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
                ( LPSTR )&sMessage,
                0,
                NULL
            );

            sReturn = sReturn.Format( "%u: %s", unsigned( nCode ), ( LPCSTR )sMessage );

            LocalFree( sMessage );
        }

        return sReturn;
    }

    /**
    * @internal
    * @brief    convert string to value
    * @tparam   tDataType datatype of value
    * @param    value
    * @return   string
    */
    template <typename tDataType>
    static string ToString( const tDataType& value )
    {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    };

    /**
    * @internal
    * @brief    convert value to string
    * @tparam   tDataType datatype of value
    * @param    string
    * @return   value
    */
    template<typename tDataType>
    static tDataType ParseString( const char* c )
    {
        tDataType value;
        std::istringstream tmp_conv( c );
        tmp_conv >> value;

        return value;
    };

    template<>
    static Vec3 ParseString<Vec3>( const char* c )
    {
        Vec3 v;

        std::istringstream tmp_conv;
        std::vector<string> temp;

        stringExplode( c, ",", temp );
        assert( temp.size() == 3 );

        // x-Value
        if ( temp.size() >= 1 )
        {
            tmp_conv.str( temp[0].c_str() );
            tmp_conv >> v.x;
        }

        // y-Value
        if ( temp.size() >= 2 )
        {
            tmp_conv.str( temp[1].c_str() );
            tmp_conv >> v.y;
        }

        // z-Value
        if ( temp.size() >= 3 )
        {
            tmp_conv.str( temp[2].c_str() );
            tmp_conv >> v.z;
        }

        return v;
    };

    template<>
    static bool ParseString<bool>( const char* c )
    {
        bool value = false;

        // "1" and "true" are interpreted as true, "0" and "false" as false
        string sValue = c;
        sValue.MakeLower();

        if ( sValue == "1" || sValue == STRING_TRUE )
        {
            value = true;
        }

        else if ( sValue == "0" || sValue == STRING_FALSE )
        {
            value = false;
        }

        return value;
    }
}