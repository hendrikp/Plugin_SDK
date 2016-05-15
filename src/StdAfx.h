/* CE3 Plugin Manager - for licensing and copyright see license.txt */

#pragma once

#include "EngineVersion.h"

// Insert your headers here
#if USE_NEW_INCLUDE_PATHS
#include <CryCore/Project/CryModuleDefs.h>
#define eCryModule eCryM_System
#include <CryCore/Platform/platform.h>
#else
#include <platform.h>
#endif

#include <algorithm>
#include <vector>
#include <memory>
#include <list>
#include <functional>
#include <limits>

#if USE_NEW_INCLUDE_PATHS
#include <CryCore/smartptr.h>
#include <CryThreading/CryThread.h>
#include <CryMath/Cry_Math.h>
#include <CrySystem/ISystem.h>
#include <Cry3DEngine/I3DEngine.h>
#include <CryInput/IInput.h>
#include <CrySystem/IConsole.h>
#include <CrySystem/ITimer.h>
#include <CrySystem/ILog.h>
#include <CryNetwork/ISerialize.h>
#else
#include <smartptr.h>
#include <CryThread.h>
#include <Cry_Math.h>
#include <ISystem.h>
#include <I3DEngine.h>
#include <IInput.h>
#include <IConsole.h>
#include <ITimer.h>
#include <ILog.h>
#include <ISerialize.h>
#endif

#include <IGameplayRecorder.h>

#ifndef _FORCEDLL
#define _FORCEDLL
#endif

#ifndef PLUGINMANAGER_EXPORTS
#define PLUGINMANAGER_EXPORTS
#endif

#pragma warning(disable: 4018)  // conditional expression is constant

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
