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
  pFuncs->setvalue      = NULL; //NPP_SetValue;
}


NP_EXPORT(NPError)
NP_Initialize(NPNetscapeFuncs* bFuncs, NPPluginFuncs* pFuncs) {
	//DBGLOG(LOG_INFO, "NP_Initialize");

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
	//DBGLOG(LOG_INFO, "NP_Shutdown");
	return NPERR_NO_ERROR;
}

/**
 * New plugin instance
 */
NPError
NPP_New(NPMIMEType pluginType, NPP instance, uint16_t mode, int16_t argc, char* argn[], char* argv[], NPSavedData* saved) {

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

	return NPERR_NO_ERROR;
}

NPError
NPP_Destroy(NPP instance, NPSavedData** save) {

  InstanceData *instanceData = (InstanceData*)(instance->pdata);
  if (NULL!=instanceData->npo) {
	  ((NPBrowser *)instanceData->npo)->Deallocate();
	  delete instanceData->npo;
  }

  free(instanceData);
  return NPERR_NO_ERROR;
}

NPError
NPP_GetValue(NPP instance, NPPVariable variable, void *value) {

	//DBGLOG(LOG_INFO, "NPP_GetValue, variable: %i", variable);

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
			instanceData->npo = (*sBrowserFuncs->createobject)(instance, &NPBrowser::_npclass);
			*(NPObject **)value = instanceData->npo;
		}
		break;
	default:
		rv = NPERR_GENERIC_ERROR;
	}
	return rv;
}

/*
NPError
NPP_SetValue(NPP instance, NPNVariable variable, void *value) {
  return NPERR_GENERIC_ERROR;
}
*/
