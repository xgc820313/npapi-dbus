/**
 * @file idbus.h
 *
 * @date   2009-09-26
 * @author jldupont
 *
 * DBus -->  NPAPI
 *
 */
#ifndef IDBUS_H_
#define IDBUS_H_

	#include "npapi.h"
	#include "npruntime.h"

    #include "dbus/dbus.h"
    #include "queue.h"

    #include <list>
	using namespace std;



	class DBusMsgNPObject : public NPObject
	{
	protected:
		// Class member functions that do the real work
		void Deallocate();
		void Invalidate();
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
		DBusMsgNPObject(NPP npp);
		~DBusMsgNPObject();

		// This is the method used to create the NPObject
		// This method should not be called explicitly
		// Instead, use NPN_CreateObject
		static NPObject* Allocate(NPP npp, NPClass *aClass) {
			return (NPObject *)new DBusMsgNPObject(npp);
		}

		/////////////////////////////
		// Static NPObject methods //
		/////////////////////////////
		static void _Deallocate(NPObject *npobj);
		static void _Invalidate(NPObject *npobj);
		static bool _HasMethod(NPObject *npobj, NPIdentifier name);
		static bool _Invoke(NPObject *npobj, NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result);
		static bool _InvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result);
		static bool _HasProperty(NPObject * npobj, NPIdentifier name);
		static bool _GetProperty(NPObject *npobj, NPIdentifier name, NPVariant *result);
		static bool _SetProperty(NPObject *npobj, NPIdentifier name, const NPVariant *value);
		static bool _RemoveProperty(NPObject *npobj, NPIdentifier name);
		static bool _Enumerate(NPObject *npobj, NPIdentifier **identifier, uint32_t *count);
		static bool _Construct(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result);

		static NPClass _npclass;

	protected:
		NPP m_Instance;
	};




	// DBus primitives & compound types
	typedef enum _vType {
		TINVALID=0,
		STRING,
		SIGNATURE,
		OBJECT_PATH,
		INT16,
		UINT16,
		INT32,
		UINT32,
		INT64,
		UINT64,
		DOUBLE,
		BYTE,
		BOOLEAN,
		VARIANT,
		//==COMPOUND==
		ARRAY,
		DICT_ENTRY,
		STRUCT
	} vType;

	//just forward declaration.
	class dArrayStruct;

	typedef union {
		char              *string;
		char              *signature;
		char              *object_path;
		unsigned short     uint16;
		short              int16;
		unsigned long      uint32;
		long               int32;
		unsigned long long uint64;
		long long          int64;
		double             dbl;
		char               byte;
		char               boolean;
	} dPrimitive;

	/**
	 * Serves to handle the primitive DBus types
	 *
	 * The field 'value' contains the value for the
	 * primitive types.
	 *
	 * The VARIANT type is handled by:
	 *   type=VARIANT
	 *   subType=STRING | ...
	 *
	 * The DICT_ENTRY primitive is handled by:
	 *   type=DICT_ENTRY
	 *   subType= type of key (key can only be a primitive type)
	 *
	 */
	class BaseValue {

		vType type;
		vType subType; //used for variant & dict_entry key

		union {
			dPrimitive   pvalue;
			dArrayStruct *arst;
			union {
				dPrimitive key;
				BaseValue  *value;
			} dict;
		} value;

	public:
		BaseValue() {
			type=TINVALID;
			subType=TINVALID;
			value.arst=NULL;
		}
		virtual ~BaseValue() {}
	};

	/**
	 * Serves to handle both DBus compound types
	 *   ARRAY and STRUCT
	 */
	class dArrayStruct {
		vType type;
		list<BaseValue> array;

	public:
		dArrayStruct() {
			type=TINVALID;
		};
		virtual ~dArrayStruct() {};
	};



	typedef enum {
		MINVALID = 0,
		DISCONNECTED,
		UNIQNAME,
		DBUSMSG
	} iMsgType;


	class BaseMsg {

	public:
		iMsgType mtype;

		BaseMsg() {
			mtype=MINVALID;
		}

		virtual ~BaseMsg() {}
	};


	class iMsg: public BaseMsg {

		iMsg():BaseMsg() {};
		~iMsg();

	};



#endif /* IDBUS_H_ */
