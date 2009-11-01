/**
 * @file npobject_browser.h
 *
 *  Created on: 2009-11-01
 *      Author: jldupont
 */

#ifndef NPOBJECT_BROWSER_H_
#define NPOBJECT_BROWSER_H_

  #include "npapi.h"
  #include "npruntime.h"

/*
 struct NPClass
{
    uint32_t structVersion;
    NPAllocateFunctionPtr allocate;
    NPDeallocateFunctionPtr deallocate;
    NPInvalidateFunctionPtr invalidate;
    NPHasMethodFunctionPtr hasMethod;
    NPInvokeFunctionPtr invoke;
    NPInvokeDefaultFunctionPtr invokeDefault;
    NPHasPropertyFunctionPtr hasProperty;
    NPGetPropertyFunctionPtr getProperty;
    NPSetPropertyFunctionPtr setProperty;
    NPRemovePropertyFunctionPtr removeProperty;
    NPEnumerationFunctionPtr enumerate;
    NPConstructFunctionPtr construct;
};

 */

  class NPBrowser {

  public:
	  NPBrowser();
	  virtual ~NPBrowser();


  protected:
	  void Deallocate(void);
	  void Invalidate(void);
	  bool HasMethod(NPIdentifier name);
	  bool Invoke(NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result);
	  bool InvokeDefault(const NPVariant *args, uint32_t argCount, NPVariant *result);
	  bool HasProperty(NPIdentifier name);
	  bool GetProperty(NPIdentifier name, NPVariant *result);
	  bool SetProperty(NPIdentifier name, const NPVariant *value);
	  bool RemoveProperty(NPIdentifier name);
	  bool Enumerate(NPIdentifier **identifier, uint32_t *count);
	  bool Construct(const NPVariant *args, uint32_t argCount, NPVariant *result);


  public:

	  static NPObject* Allocate(NPP npp, NPClass *aClass) {
	          return (NPObject *)new MyScriptableNPObject(npp);
	  }

	  static NPClass _npclass;

	  /*
	   * NPClass interface
	   */
	  static NPObject *(*_NPAllocateFunctionPtr)(NPP npp, NPClass *aClass);
	  static void (*_NPDeallocateFunctionPtr)(NPObject *npobj);
	  static void (*_NPInvalidateFunctionPtr)(NPObject *npobj);
	  static bool (*_NPHasMethodFunctionPtr)(NPObject *npobj, NPIdentifier name);
	  static bool (*_NPInvokeFunctionPtr)(NPObject *npobj, NPIdentifier name,
	                                      const NPVariant *args, uint32_t argCount,
	                                      NPVariant *result);
	  static bool (*_NPInvokeDefaultFunctionPtr)(NPObject *npobj,
	                                             const NPVariant *args,
	                                             uint32_t argCount,
	                                             NPVariant *result);
	  static bool (*_NPHasPropertyFunctionPtr)(NPObject *npobj, NPIdentifier name);
	  static bool (*_NPGetPropertyFunctionPtr)(NPObject *npobj, NPIdentifier name,
	                                           NPVariant *result);
	  static bool (*_NPSetPropertyFunctionPtr)(NPObject *npobj, NPIdentifier name,
	                                           const NPVariant *value);
	  static bool (*_NPRemovePropertyFunctionPtr)(NPObject *npobj,
	                                              NPIdentifier name);
	  static bool (*_NPEnumerationFunctionPtr)(NPObject *npobj, NPIdentifier **value,
	                                           uint32_t *count);
	  static bool (*_NPConstructFunctionPtr)(NPObject *npobj,
	                                         const NPVariant *args,
	                                         uint32_t argCount,
	                                         NPVariant *result);

  };

#endif /* NPOBJECT_BROWSER_H_ */
