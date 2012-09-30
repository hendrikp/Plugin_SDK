/* CE3 Plugin Manager - for licensing and copyright see license.txt */

#pragma once

#define _FORCEDLL

#ifndef PLUGINMANAGER_EXPORTS
#define PLUGINMANAGER_EXPORTS
#endif

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

#pragma warning(disable: 4018)  // conditional expression is constant

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
