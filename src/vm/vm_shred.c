#include <stdlib.h>
#include <string.h>
#include "vm.h"
#include "type.h"
#include "instr.h"
#include "mpool.h"

struct Stack_ { char c[SIZEOF_REG]; char d[SIZEOF_MEM]; };
POOL_HANDLE(VM_Shred, 512)
POOL_HANDLE(Stack, 512)

VM_Shred new_vm_shred(VM_Code c) {
  VM_Shred shred    = mp_alloc(VM_Shred);
  shred->_reg       = mp_alloc(Stack);
  shred->base       = shred->_reg + SIZEOF_REG;
  shred->reg        = shred->_reg;
  shred->mem        = shred->base;
  shred->_mem       = shred->base;
  shred->code       = c;
  shred->name       = strdup(c->name);
  vector_init(&shred->gc1);
  return shred;
}

static void vm_shred_free_args(Vector v) {
  LOOP_OPTIM
  for(m_uint i = vector_size(v) + 1; --i;)
    free((void*)vector_at(v, i - 1));
  free_vector(v);
}

void free_vm_shred(VM_Shred shred) {
  release(shred->me, shred);
  mp_free(Stack, shred->_reg);
  REM_REF(shred->code);
  free(shred->name);
  vector_release(&shred->gc1);
  if(shred->args)
    vm_shred_free_args(shred->args);
  mp_free(VM_Shred, shred);
}
