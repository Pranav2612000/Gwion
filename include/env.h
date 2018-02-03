#ifndef __ENV
#define __ENV
#include "absyn.h"

struct Env_ {
  Nspc curr;
  Nspc global_nspc;
//  Nspc user_nspc;
  m_uint    class_scope;
  Context   context;
  Type      class_def;
  Func      func;
  Exp_Func* current; // template helper
  m_uint type_xid;
  struct Vector_    contexts;
  struct Vector_    nspc_stack;
  struct Vector_    class_stack;
  struct Vector_    breaks;
  struct Vector_    conts;
  struct Vector_    known_ctx;
};

Env new_env();
void env_reset(Env);
void free_env(Env);
m_bool env_push_class(Env, Type);
m_bool env_pop_class(Env);
#define SCOPE(a) env->class_scope++;a;env->class_scope--;
#define NSPC(a) env->class_scope++;nspc_push_value(env->curr);a;\
nspc_pop_value(env->curr);env->class_scope--;

Map env_label(Env);
Nspc env_nspc(Env);
Class_Def env_class_def(Env, Class_Def);
Type scan_type(Env, Type, Type_Decl*);
m_bool compat_func(Func_Def, Func_Def, int);
m_bool type_unknown(ID_List, m_str);
#endif
