/**
 * @file npobject_browser.cc
 *
 *  Created on: 2009-11-01
 *      Author: jldupont
 */

#include <string.h>
#include <stdlib.h>
#include "npobject_browser.h"

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
	return ((NPBrowser*)obj)->HasMethod(name);
}
bool NPBrowser::_NPInvoke(NPObject *obj, NPIdentifier name, const NPVariant *args, uint32_t argCount,NPVariant *result) {
	return ((NPBrowser*)obj)->Invoke(name, args, argCount, result);
}
bool NPBrowser::_NPInvokeDefault(NPObject *obj,const NPVariant *args,uint32_t argCount,NPVariant *result) {
	return ((NPBrowser*)obj)->InvokeDefault(args, argCount, result);
}
bool NPBrowser::_NPHasProperty(NPObject *obj, NPIdentifier name) {
	return ((NPBrowser*)obj)->HasProperty(name);
}
bool NPBrowser::_NPGetProperty(NPObject *obj, NPIdentifier name,NPVariant *result) {
	return ((NPBrowser*)obj)->GetProperty(name, result);
}
bool NPBrowser::_NPSetProperty(NPObject *obj, NPIdentifier name,const NPVariant *value) {
	return ((NPBrowser*)obj)->SetProperty(name, value);
}
bool NPBrowser::_NPRemoveProperty(NPObject *obj,NPIdentifier name) {
	return ((NPBrowser*)obj)->RemoveProperty(name);
}
bool NPBrowser::_NPEnumeration(NPObject *obj, NPIdentifier **value,uint32_t *count) {
    return ((NPBrowser*)obj)->Enumeration(value, count);
}
bool NPBrowser::_NPConstruct(NPObject *obj,const NPVariant *args,uint32_t argCount,NPVariant *result) {
    return ((NPBrowser*)obj)->Construct(args, argCount, result);
}



// --------------------------------------------------------------------------------------------
// IMPLEMENTATION
// --------------------------------------------------------------------------------------------


NPBrowser::NPBrowser(NPP npp) {
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

	NPUTF8 *nname = NPN_UTF8FromIdentifier(name);

	bool result=(strcmp(POPMSG_METHOD, nname)==0);

	NPN_MemFree(nname);

	return result;
}

bool NPBrowser::Invoke(NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result) {

	NPUTF8 *nname = NPN_UTF8FromIdentifier(name);

	// only "popmsg" method is supported.
	bool r=(strcmp(POPMSG_METHOD, nname)==0);
	NPN_MemFree(nname);

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

}

bool NPBrowser::GetProperty(NPIdentifier name, NPVariant *result) {

}

bool NPBrowser::SetProperty(NPIdentifier name, const NPVariant *value) {

}

bool NPBrowser::RemoveProperty(NPIdentifier name) {

}

bool NPBrowser::Enumeration(NPIdentifier **identifier, uint32_t *count) {

}

bool NPBrowser::Construct(const NPVariant *args, uint32_t argCount, NPVariant *result) {

}

