/**
 * @file npobject_browser.cc
 *
 *  Created on: 2009-11-01
 *      Author: jldupont
 */

#include <string.h>
#include <stdlib.h>
#include "npobject_browser.h"
#include "macros.h"
#include "plugin.h"

#define POPMSG_METHOD  "popmsg"

NPClass NPBrowser::_npclass = {
    NP_CLASS_STRUCT_VERSION,
    NPBrowser::Allocate,
    NPBrowser::_NPDeallocate,
    NPBrowser::_NPInvalidate,
    NPBrowser::_NPHasMethod,
    NPBrowser::_NPInvoke,
    NPBrowser::_NPInvokeDefault,
    NPBrowser::_NPHasProperty,
    NPBrowser::_NPGetProperty,
    NPBrowser::_NPSetProperty,
    NPBrowser::_NPRemoveProperty,
    NPBrowser::_NPEnumeration,
    NPBrowser::_NPConstruct
};

void NPBrowser::_NPInvalidate(NPObject *obj) {
    ((NPBrowser*)obj)->Invalidate();
}
// static
void NPBrowser::_NPDeallocate(NPObject *obj) {
    ((NPBrowser*)obj)->Deallocate();
    delete ((NPBrowser*)obj);
}
bool NPBrowser::_NPHasMethod(NPObject *obj, NPIdentifier name) {

	DBGLOG(LOG_INFO, "NPBrowser::_NPHasMethod");

	return ((NPBrowser*)obj)->HasMethod(name);
}
bool NPBrowser::_NPInvoke(NPObject *obj, NPIdentifier name, const NPVariant *args, uint32_t argCount,NPVariant *result) {

	DBGLOG(LOG_INFO, "NPBrowser::_NPInvoke");

	return ((NPBrowser*)obj)->Invoke(name, args, argCount, result);
}
bool NPBrowser::_NPInvokeDefault(NPObject *obj,const NPVariant *args,uint32_t argCount,NPVariant *result) {
	DBGLOG(LOG_INFO, "NPBrowser::_NPInvokeDefault");

	return ((NPBrowser*)obj)->InvokeDefault(args, argCount, result);
}
bool NPBrowser::_NPHasProperty(NPObject *obj, NPIdentifier name) {

	DBGLOG(LOG_INFO, "NPBrowser::_NPHasProperty, name: %i", name);

	return ((NPBrowser*)obj)->HasProperty(name);
}
bool NPBrowser::_NPGetProperty(NPObject *obj, NPIdentifier name,NPVariant *result) {

	DBGLOG(LOG_INFO, "NPBrowser::_NPGetProperty, name: %i", name);

	return ((NPBrowser*)obj)->GetProperty(name, result);
}
bool NPBrowser::_NPSetProperty(NPObject *obj, NPIdentifier name,const NPVariant *value) {
	return ((NPBrowser*)obj)->SetProperty(name, value);
}
bool NPBrowser::_NPRemoveProperty(NPObject *obj,NPIdentifier name) {
	return ((NPBrowser*)obj)->RemoveProperty(name);
}
bool NPBrowser::_NPEnumeration(NPObject *obj, NPIdentifier **value,uint32_t *count) {

	DBGLOG(LOG_INFO, "NPBrowser::_NPEnumeration");
    return ((NPBrowser*)obj)->Enumeration(value, count);
}
bool NPBrowser::_NPConstruct(NPObject *obj,const NPVariant *args,uint32_t argCount,NPVariant *result) {

	DBGLOG(LOG_INFO, "NPBrowser::_NPConstruct");
    return ((NPBrowser*)obj)->Construct(args, argCount, result);
}



// --------------------------------------------------------------------------------------------
// IMPLEMENTATION
// --------------------------------------------------------------------------------------------


NPBrowser::NPBrowser(NPP npp) {

	DBGLOG(LOG_INFO, "NPBrowser::NPBrowser");

	m_Instance=npp;
	mb=NULL;
}//

NPBrowser::~NPBrowser() {}

void NPBrowser::Deallocate() {
    if (NULL!=mb) {
    	mb->release();
    	delete mb;
    }
}

void NPBrowser::Invalidate() {
    // Invalidate the control however you wish
}

bool NPBrowser::HasMethod(NPIdentifier name) {

	DBGLOG(LOG_INFO, "NPBrowser::HasMethod, name: %i", name );
	InstanceData* instanceData = (InstanceData *)m_Instance->pdata;

	NPUTF8 *nname = (*instanceData->sBrowserFuncs->utf8fromidentifier)(name);

	DBGLOG(LOG_INFO, "NPBrowser::HasMethod, nname<%s>", nname );

	bool result=(strcmp(POPMSG_METHOD, nname)==0);

	DBGLOG(LOG_INFO, "NPBrowser::HasMethod, result<%i>", result );

	(*instanceData->sBrowserFuncs->memfree)(nname);

	return result;
}

bool NPBrowser::Invoke(NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result) {

	DBGLOG(LOG_INFO, "NPBroser::Invoke");

	InstanceData* instanceData = (InstanceData *)m_Instance->pdata;

	NPUTF8 *nname = (*instanceData->sBrowserFuncs->utf8fromidentifier)(name);

	// only "popmsg" method is supported.

	bool r=(strcmp(POPMSG_METHOD, nname)==0);
	(*instanceData->sBrowserFuncs->memfree)(nname);

	if (!r)
		return false;

	char str[] = "json response";
	char *pstr = (char *) malloc(sizeof(str)+1);
	strcpy(pstr, str);

	STRINGZ_TO_NPVARIANT(pstr, *result);

	return true;
}//

bool NPBrowser::InvokeDefault(const NPVariant *args, uint32_t argCount, NPVariant *result) {

}

bool NPBrowser::HasProperty(NPIdentifier name) {

	DBGLOG(LOG_INFO, "NPBrowser::HasProperty, name: %i", name);

}

bool NPBrowser::GetProperty(NPIdentifier name, NPVariant *result) {

	DBGLOG(LOG_INFO, "NPBrowser::GetProperty, name: %i", name);

}

bool NPBrowser::SetProperty(NPIdentifier name, const NPVariant *value) {

}

bool NPBrowser::RemoveProperty(NPIdentifier name) {

}

bool NPBrowser::Enumeration(NPIdentifier **identifier, uint32_t *count) {

}

bool NPBrowser::Construct(const NPVariant *args, uint32_t argCount, NPVariant *result) {

}

