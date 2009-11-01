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

  class NPBrowser: public NPObject {

  public:
	  NPBrowser(NPP npp);
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
	  bool Enumeration(NPIdentifier **identifier, uint32_t *count);
	  bool Construct(const NPVariant *args, uint32_t argCount, NPVariant *result);


  public:

	  static NPObject* Allocate(NPP npp, NPClass *aClass) {
	          return (NPObject *)new NPBrowser(npp);
	  }

	  static NPClass _npclass;

	  /*
	   * NPClass interface
	   */
	  static NPObject *(*_NPAllocateFunctionPtr)(NPP npp, NPClass *aClass);

	  static void _NPDeallocate(NPObject *npobj);
	  static void _NPInvalidate(NPObject *npobj);
	  static bool _NPHasMethod(NPObject *npobj, NPIdentifier name);
	  static bool _NPInvoke(NPObject *npobj, NPIdentifier name, const NPVariant *args, uint32_t argCount,NPVariant *result);
	  static bool _NPInvokeDefault(NPObject *npobj,const NPVariant *args,uint32_t argCount,NPVariant *result);
	  static bool _NPHasProperty(NPObject *npobj, NPIdentifier name);
	  static bool _NPGetProperty(NPObject *npobj, NPIdentifier name,NPVariant *result);
	  static bool _NPSetProperty(NPObject *npobj, NPIdentifier name,const NPVariant *value);
	  static bool _NPRemoveProperty(NPObject *npobj,NPIdentifier name);
	  static bool _NPEnumeration(NPObject *npobj, NPIdentifier **value,uint32_t *count);
	  static bool _NPConstruct(NPObject *npobj,const NPVariant *args,uint32_t argCount,NPVariant *result);

  protected:
      NPP m_Instance;
  };

#endif /* NPOBJECT_BROWSER_H_ */
