#include <stdlib.h>
#include <stdio.h>
#include "map.h"
#include "context.h"
#include "func.h"
#include "oo.h"
#include "emit.h"

Value new_value(Type type, m_str name) {
  Value a               = (Value)calloc(1, sizeof(struct Value_));
  a->m_type             = type;
  a->name               = name;
  INIT_OO(a, e_value_obj);
  return a;
}

static void free_value(Value a) {
  if(a->ptr) {
    if(isprim(a->m_type) > 0 && !GET_FLAG(a, ae_flag_enum))
      free(a->ptr);
  }
  free(a);
}

Func new_func(m_str name, Func_Def def) {
  Func func = calloc(1, sizeof(struct Func_));
  func->name = name;
  func->def = def;
  INIT_OO(func, e_func_obj);
  return func;
}

static void free_func(Func a) {
  if(a->code) {
    if(a->def && !GET_FLAG(a, ae_flag_template)) {
      if(!GET_FLAG(a->def, ae_flag_dtor)) {
        REM_REF(a->code);
      }
    }
  }
  if(a->def && !GET_FLAG(a, ae_flag_template))
    free_func_def(a->def);
  free(a);
}

Type new_type(te_type xid, m_str name, Type parent) {
  Type type    = calloc(1, sizeof(struct Type_));
  type->xid    = xid;
  type->name   = name;
  type->parent = parent;
  INIT_OO(type, e_type_obj);
  return type;
}

static void free_type(Type a) {
  if(a->info)
    REM_REF(a->info);
  if(!GET_FLAG(a, ae_flag_builtin) || a->parent == &t_int || isa(a, &t_class) > 0
      || isa(a, &t_function) > 0 || a->array_depth)
    free(a);
}

Type type_copy(Env env, Type type) {
  Type a         = calloc(1, sizeof(struct Type_));
  a->xid         = type->xid;
  a->name        = type->name;
  a->parent      = type->parent;
  a->info        = type->info;
  a->owner       = type->owner;
  a->size        = type->size;
  a->d.actual_type = type->d.actual_type;
  a->array_depth = type->array_depth;
  if(GET_FLAG(type, ae_flag_checked))
    SET_FLAG(a, ae_flag_checked);
  a->def         = type->def;
  INIT_OO(a, e_type_obj);
  return a;
}

void rem_ref(VM_Object a, void* ptr) {
  if(!--a->ref_count) {
    switch(a->type) {
      case e_nspc_obj:
        free_nspc(ptr);
        break;
      case e_type_obj:
        free_type(ptr);
        break;
      case e_value_obj:
        free_value(ptr);
        break;
      case e_context_obj:
        free_context(ptr);
        break;
      case e_func_obj:
        free_func(ptr);
        break;
      case e_code_obj:
        free_vm_code(ptr);
        break;
    }
  }
}