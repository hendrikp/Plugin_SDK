/* CE3 Plugin Manager - for licensing and copyright see license.txt */

#pragma once

#include <CryModuleDefs.h>
#define eCryModule eCryM_PluginManager
#define RWI_NAME_TAG "RayWorldIntersection(Game)"
#define PWI_NAME_TAG "PrimitiveWorldIntersection(Game)"

// Insert your headers here
#include <platform.h>
#include <algorithm>
#include <vector>
#include <memory>
#include <list>
#include <functional>
#include <limits>
#include <smartptr.h>
#include <CryThread.h>
#include <Cry_Math.h>
#include <ISystem.h>
#include <I3DEngine.h>
#include <IInput.h>
#include <IConsole.h>
#include <ITimer.h>
#include <ILog.h>
#include <IGameplayRecorder.h>
#include <ISerialize.h>

#ifndef PLUGINMANAGER_EXPORTS
#define PLUGINMANAGER_EXPORTS
#endif

#ifdef PLUGINMANAGER_EXPORTS
#define GAME_API DLL_EXPORT
#else
#define GAME_API DLL_IMPORT
#endif

#pragma warning(disable: 4018)  // conditional expression is constant

inline void GameWarning( const char* format, ... ) PRINTF_PARAMS( 1, 2 );
inline void GameWarning( const char* format, ... )
{
    if ( !format )
    {
        return;
    }

    va_list args;
    va_start( args, format );
    GetISystem()->WarningV( VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, 0, NULL, format, args );
    va_end( args );
}

#define PLAYER_REFACTORING 1

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
