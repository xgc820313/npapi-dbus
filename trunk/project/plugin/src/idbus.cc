/**
 * @file idbus.cc
 *
 * @date   2009-09-26
 * @author jldupont
 *
 * DBus -->  NPAPI
 *
 */
#include "idbus.h"

DBusMsgNPObject::DBusMsgNPObject(NPP npp):NPObject() {

}

DBusMsgNPObject::~DBusMsgNPObject() {

}

void DBusMsgNPObject::_Invalidate(NPObject *obj) {
    ((DBusMsgNPObject*)obj)->Invalidate();
}
void DBusMsgNPObject::Invalidate() {
    // Invalidate the control however you wish
}

// static
void DBusMsgNPObject::_Deallocate(NPObject *obj) {
    ((DBusMsgNPObject*)obj)->Deallocate();
    delete ((DBusMsgNPObject*)obj);
}
void DBusMsgNPObject::Deallocate() {
    // Do any cleanup needed
}

NPClass DBusMsgNPObject::_npclass = {
    NP_CLASS_STRUCT_VERSION,
    DBusMsgNPObject::Allocate,
    DBusMsgNPObject::_Deallocate,
    DBusMsgNPObject::_Invalidate,
    DBusMsgNPObject::_HasMethod,
    DBusMsgNPObject::_Invoke,
    DBusMsgNPObject::_InvokeDefault,
    DBusMsgNPObject::_HasProperty,
    DBusMsgNPObject::_GetProperty,
    DBusMsgNPObject::_SetProperty,
    DBusMsgNPObject::_RemoveProperty,
    DBusMsgNPObject::_Enumerate,
    DBusMsgNPObject::_Construct
};
