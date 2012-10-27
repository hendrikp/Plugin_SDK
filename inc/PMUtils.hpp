/* CE3 Plugin Manager - for licensing and copyright see license.txt */

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

#ifndef PATH_SEPERATOR
#define PATH_SEPERATOR "\\"
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
    static string UTF82ACP( const string sSource )
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

    /**
    * @internal
    * @brief delayed function type
    */
    typedef void ( *tDelayedCall )( void* );

    /**
    * @internal
    * @brief class to delay function calls
    */
    class CallDelay
    {
        public:
            /**
            * @internal
            * @brief delay type enum
            */
            enum eDelayType
            {
                eDT_None = 0, //!< initialization
                eDT_Frames = 1, //!< frame based delay
                eDT_Seconds = 2, //!< time based delay
                eDT_Default = eDT_Frames
            };

        protected:
            tDelayedCall m_pFunc; //!< function pointer delayed function
            void* m_pData; //!< data for delayed function
            eDelayType m_eDT; //!< type of delay
            float m_fLeft; //!< delay amount left

        private:
            CallDelay() {};

        public:
            /**
            * @internal
            * @brief CallDelay constructor
            * @param pFunc function pointer of delayed function
            * @param pData Data for delayed function
            * @param fDelay delay amount
            * @param eType delay type
            */
            CallDelay( tDelayedCall pFunc, void* pData = NULL, float fDelay = 1.0f, eDelayType eType = eDT_Default )
            {
                m_pFunc = pFunc;
                m_pData = pData;
                m_eDT = eType;
                m_fLeft = fDelay;
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
                    m_pFunc( m_pData );
                    return true;
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
            std::list<CallDelay> m_queue;

        public:
            /*
            * @internal
            * @brief Delay a function
            * @param pFunc function pointer of delayed function
            * @param pData Data for delayed function
            * @param fDelay delay amount
            * @param eType delay type
            */
            void DelayFunction( tDelayedCall pFunc, void* pData = NULL, float fDelay = 1.0f, CallDelay::eDelayType eType = CallDelay::eDT_Default )
            {
                m_queue.push_back( CallDelay( pFunc, pData, fDelay, eType ) );
            }

            /**
            * @internal
            * @brief Advance all delayed calls in this queue
            * @param fDeltaTime time passed in seconds
            */
            void AdvanceAll( float fDeltaTime )
            {
                bool bFinished = false;

                for ( auto qIter = m_queue.begin(); qIter != m_queue.end(); ++qIter )
                {
                    bFinished = ( *qIter ).Advance( fDeltaTime );

                    // Execution of this function is finished
                    if ( bFinished )
                    {
                        qIter = m_queue.erase( qIter );

                        if ( qIter == m_queue.end() )
                        {
                            break;
                        }
                    }
                }
            }
    };

    /**
    * @internal
    * @brief Delayed Console Commands
    * @param sCommand to execute and delete[]
    */
    static void _DelayedConsoleCommand( void* sCommand )
    {
        if ( sCommand )
        {
            if ( gEnv && gEnv->pConsole )
            {
                gEnv->pConsole->ExecuteString( ( const char* )sCommand );
            }

            delete [] ( const char* )( sCommand );
        }
    }

    /**
    * @internal
    * @brief Delays a console command
    * Since the built in defer didn't work as expected
    * @param qDelayer Delay Queue that handles this command
    * @param sCommand the console command
    * @param fDelay delay amount
    * @param eType delay type
    */
    static void DelayConsoleCommand( CallDelayQueue& qDelayer, string sCommand, float fDelay = 1, CallDelay::eDelayType eType = CallDelay::eDT_Default )
    {
        if ( sCommand.length() > 0 )
        {
            // Prepare data
            size_t nLen = sCommand.length() + 1;
            char* sCmd = new char[nLen];
            strncpy( sCmd, sCommand.c_str(), nLen );
            sCmd[nLen - 1] = 0;

            qDelayer.DelayFunction( &_DelayedConsoleCommand, sCmd, fDelay, eType );
        }
    }
}