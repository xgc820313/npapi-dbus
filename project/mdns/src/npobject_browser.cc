/**
 * @file npobject_browser.cc
 *
 *  Created on: 2009-11-01
 *      Author: jldupont
 */

#include "npobject_browser.h"


NPClass NPBrowser::_npclass = {
    NP_CLASS_STRUCT_VERSION,
    NPBrowser::Allocate,
    NPBrowser::_Deallocate,
    NPBrowser::_Invalidate,
    NPBrowser::_HasMethod,
    NPBrowser::_Invoke,
    NPBrowser::_InvokeDefault,
    NPBrowser::_HasProperty,
    NPBrowser::_GetProperty,
    NPBrowser::_SetProperty,
    NPBrowser::_RemoveProperty,
    NPBrowser::_Enumerate,
    NPBrowser::_Construct
};
