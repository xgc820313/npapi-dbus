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


#define PLUGIN_NAME        "DBus-mdns adapter"
#define PLUGIN_DESCRIPTION PLUGIN_NAME " Serves as 'dbus-mdns ServiceBrowser for discovering HTTP services"
#define PLUGIN_VERSION     "1.0.0.0"
#define PLUGIN_MIME        "application/dbus-mdns:dbmdns:dbus-mdns"

static NPNetscapeFuncs* sBrowserFuncs = NULL;



static void
fillPluginFunctionTable(NPPluginFuncs* pFuncs)
{
  pFuncs->version = 11;
  pFuncs->size = sizeof(*pFuncs);
  pFuncs->newp          = NPP_New;
  pFuncs->destroy       = NPP_Destroy;
  pFuncs->setwindow     = NPP_SetWindow;
  pFuncs->newstream     = NPP_NewStream;
  pFuncs->destroystream = NPP_DestroyStream;
  pFuncs->asfile        = NPP_StreamAsFile;
  pFuncs->writeready    = NPP_WriteReady;
  pFuncs->write         = NPP_Write;
  pFuncs->print         = NPP_Print;
  pFuncs->event         = NPP_HandleEvent;
  pFuncs->urlnotify     = NPP_URLNotify;
  pFuncs->getvalue      = NPP_GetValue;
  pFuncs->setvalue      = NPP_SetValue;
}


NP_EXPORT(NPError)
NP_Initialize(NPNetscapeFuncs* bFuncs, NPPluginFuncs* pFuncs)
{
  sBrowserFuncs = bFuncs;

  fillPluginFunctionTable(pFuncs);

  return NPERR_NO_ERROR;
}

NP_EXPORT(char*)
NP_GetPluginVersion()
{
  return PLUGIN_VERSION;
}

NP_EXPORT(char*)
NP_GetMIMEDescription()
{
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

  return NPERR_NO_ERROR;
}

NPError
NPP_Destroy(NPP instance, NPSavedData** save) {
  InstanceData* instanceData = (InstanceData*)(instance->pdata);

  // @TODO

  free(instanceData);
  return NPERR_NO_ERROR;
}

NPError
NPP_SetWindow(NPP instance, NPWindow* window) {
  InstanceData* instanceData = (InstanceData*)(instance->pdata);
  instanceData->window = *window;
  return NPERR_NO_ERROR;
}

NPError
NPP_NewStream(NPP instance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16_t* stype) {
  return NPERR_GENERIC_ERROR;
}

NPError
NPP_DestroyStream(NPP instance, NPStream* stream, NPReason reason) {
  return NPERR_GENERIC_ERROR;
}

int32_t
NPP_WriteReady(NPP instance, NPStream* stream) {
  return 0;
}

int32_t
NPP_Write(NPP instance, NPStream* stream, int32_t offset, int32_t len, void* buffer) {
  return 0;
}

void NPP_StreamAsFile(NPP instance, NPStream* stream, const char* fname) {}
void NPP_Print(NPP instance, NPPrint* platformPrint) {}
void NPP_URLNotify(NPP instance, const char* URL, NPReason reason, void* notifyData) {}

NPError
NPP_GetValue(NPP instance, NPPVariable variable, void *value) {

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
			instanceData->npo = NPN_CreateObject(instance, &NPBrowser::_npclass);
			*(NPObject **)value = instanceData->npo;
		}
		break;
	default:
		rv = NPERR_GENERIC_ERROR;
	}

	return rv;
}

NPError
NPP_SetValue(NPP instance, NPNVariable variable, void *value) {
  return NPERR_GENERIC_ERROR;
}


int16_t
NPP_HandleEvent(NPP instance, void* event) {
  InstanceData *instanceData = (InstanceData*)(instance->pdata);

  return 1;
}
