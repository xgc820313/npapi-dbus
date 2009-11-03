/**
 * @file   plugin.cc
 *
 * @date   2009-10-26
 * @author jldupont
 *
 *
 *
 */
#pragma GCC diagnostic ignored "-Wwrite-strings"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "npapi.h"
#include "plugin.h"
#include "npobject_browser.h"
#include "macros.h"


#define PLUGIN_NAME        "DBus-mdns adapter"
#define PLUGIN_DESCRIPTION PLUGIN_NAME " Serves as 'dbus-mdns ServiceBrowser for discovering HTTP services"
#define PLUGIN_VERSION     "1.0.0.0"
#define PLUGIN_MIME        "application/dbusmdns:dbmdns:dbusmdns"

static NPNetscapeFuncs* sBrowserFuncs = NULL;


namespace
{
    class dynamic_library_load_unload_handler
    {
         public:
              dynamic_library_load_unload_handler(){
                    DBGLOG(LOG_INFO, "loaded!");
              }
              ~dynamic_library_load_unload_handler(){
                    // Code to execute when the library is unloaded
              }
    } dynamic_library_load_unload_handler_hook;
}

static void
fillPluginFunctionTable(NPPluginFuncs* pFuncs)
{
  pFuncs->version = 11;
  pFuncs->size = sizeof(*pFuncs);
  pFuncs->newp          = NPP_New;
  pFuncs->destroy       = NPP_Destroy;
  pFuncs->setwindow     = NULL; //NPP_SetWindow;
  pFuncs->newstream     = NULL; //NPP_NewStream;
  pFuncs->destroystream = NULL; //NPP_DestroyStream;
  pFuncs->asfile        = NULL; //NPP_StreamAsFile;
  pFuncs->writeready    = NULL; //NPP_WriteReady;
  pFuncs->write         = NULL; //NPP_Write;
  pFuncs->print         = NULL; //NPP_Print;
  pFuncs->event         = NULL; //NPP_HandleEvent;
  pFuncs->urlnotify     = NULL; //NPP_URLNotify;
  pFuncs->getvalue      = NPP_GetValue;
  pFuncs->setvalue      = NPP_SetValue;
}


NP_EXPORT(NPError)
NP_Initialize(NPNetscapeFuncs* bFuncs, NPPluginFuncs* pFuncs) {
	DBGLOG(LOG_INFO, "NP_Initialize");

	sBrowserFuncs = bFuncs;

	fillPluginFunctionTable(pFuncs);

	return NPERR_NO_ERROR;
}

NP_EXPORT(char*)
NP_GetPluginVersion() {
  return PLUGIN_VERSION;
}

NP_EXPORT(char*)
NP_GetMIMEDescription() {
  return PLUGIN_MIME;
}

NP_EXPORT(NPError)
NP_GetValue(void* future, NPPVariable aVariable, void* aValue) {
  switch (aVariable) {
    case NPPVpluginNameString:
      *((char**)aValue) = PLUGIN_NAME;
      break;
    case NPPVpluginDescriptionString:
      *((char**)aValue) = PLUGIN_DESCRIPTION;
      break;
    default:
      return NPERR_INVALID_PARAM;
      break;
  }
  return NPERR_NO_ERROR;
}

NP_EXPORT(NPError)
NP_Shutdown() {
	DBGLOG(LOG_INFO, "NP_Shutdown");

	return NPERR_NO_ERROR;
}

/**
 * New plugin instance
 */
NPError
NPP_New(NPMIMEType pluginType, NPP instance, uint16_t mode, int16_t argc, char* argn[], char* argv[], NPSavedData* saved) {

	DBGLOG(LOG_INFO, "NPP_New");

	sBrowserFuncs->setvalue(instance, NPPVpluginWindowBool, (void*)false);

	// set up our our instance data
	InstanceData* instanceData = (InstanceData*)malloc(sizeof(InstanceData));
	if (!instanceData)
		return NPERR_OUT_OF_MEMORY_ERROR;

	memset(instanceData, 0, sizeof(InstanceData));

	instanceData->npp = instance;
	instance->pdata = instanceData;

	instanceData->npo=NULL;
	instanceData->sBrowserFuncs = sBrowserFuncs;

	DBGLOG(LOG_INFO, "NPP_New - end");

	return NPERR_NO_ERROR;
}

NPError
NPP_Destroy(NPP instance, NPSavedData** save) {
  InstanceData* instanceData = (InstanceData*)(instance->pdata);

  // @TODO
  DBGLOG(LOG_INFO, "NPP_Destroy");

  free(instanceData);
  return NPERR_NO_ERROR;
}

NPError
NPP_GetValue(NPP instance, NPPVariable variable, void *value) {

	DBGLOG(LOG_INFO, "NPP_GetValue, variable: %i", variable);

	InstanceData *instanceData = (InstanceData*)(instance->pdata);

	NPError rv = NPERR_NO_ERROR;
	switch(variable) {
	case NPPVpluginNameString:
		value = *((char **)value) = PLUGIN_NAME;
		break;
	case NPPVpluginDescriptionString:
		*((char **)value) = PLUGIN_DESCRIPTION;
		break;
	case NPPVpluginScriptableNPObject:
		if (NULL!=instanceData->npo) {
		  *(NPObject **)value = instanceData->npo;
		} else {
			DBGLOG(LOG_INFO, " BEFORE NPN_CreateObject");
			instanceData->npo = (*sBrowserFuncs->createobject)(instance, &NPBrowser::_npclass);
			*(NPObject **)value = instanceData->npo;
			DBGLOG(LOG_INFO, " AFTER NPN_CreateObject");
		}
		break;
	default:
		rv = NPERR_GENERIC_ERROR;
	}

	DBGLOG(LOG_INFO, "NPP_GetValue - end");
	return rv;
}

NPError
NPP_SetValue(NPP instance, NPNVariable variable, void *value) {
  return NPERR_GENERIC_ERROR;
}

