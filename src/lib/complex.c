#include <math.h>
#include <complex.h>
#include "type.h"
#include "err_msg.h"
#include "instr.h"
#include "import.h"

static INSTR(complex_assign) { GWDEBUG_EXE
  POP_REG(shred, SZ_INT);
  *(m_complex*)REG(-SZ_COMPLEX) = (**(m_complex**)REG(-SZ_COMPLEX) =
     *(m_complex*)REG(SZ_INT-SZ_COMPLEX));
}

#define describe(name, op) \
static INSTR(complex_##name) { GWDEBUG_EXE \
  POP_REG(shred, SZ_COMPLEX); \
  *(m_complex*)REG(-SZ_COMPLEX) op##= *(m_complex*)REG(0); \
}
describe(plus,   +)
describe(minus,  -)
describe(mul,  *)
describe(div, /)

static INSTR(complex_r_assign) { GWDEBUG_EXE
  POP_REG(shred, SZ_INT);
  **(m_complex**)REG(0) = *(m_complex*)REG(-SZ_COMPLEX);
}

#define describe_r(name, op) \
static INSTR(complex_r_##name) { GWDEBUG_EXE \
  POP_REG(shred, SZ_INT); \
  *(m_complex*)REG(-SZ_COMPLEX) = (**(m_complex**)REG(0) op##= (*(m_complex*)REG(-SZ_COMPLEX))); \
}
describe_r(plus,  +)
describe_r(minus, -)
describe_r(mul, *)
describe_r(div, /)

INSTR(ComplexReal) { GWDEBUG_EXE
//  if(!instr->m_val) { // other case skipped in emit.c
    *(m_float*)REG(-SZ_INT) = **(m_float**)REG(-SZ_INT);
    PUSH_REG(shred, SZ_FLOAT - SZ_INT);
//  }
}

INSTR(ComplexImag) { GWDEBUG_EXE
  if(instr->m_val) {
    const m_float* f = *(m_float**)REG(-SZ_INT);
    *(const m_float**)REG(-SZ_INT) = (f + 1);
  } else {
    const m_float* f = *(m_float**)REG(-SZ_INT);
    *(m_float*)REG(-SZ_INT) = *(f + 1);
    PUSH_REG(shred, SZ_FLOAT - SZ_INT);
  }
}

#define polar_def1(name, op)                                               \
static INSTR(polar_##name) { GWDEBUG_EXE                                   \
  POP_REG(shred, SZ_COMPLEX);                                              \
  const m_complex a = *(m_complex*)REG(-SZ_COMPLEX);                       \
  const m_complex b = *(m_complex*)REG(0);                                 \
  const m_float re = creal(a) * cos(cimag(a)) op creal(b) * cos(cimag(b)); \
  const m_float im = creal(a) * sin(cimag(a)) op creal(b) * sin(cimag(b)); \
  *(m_complex*)REG(-SZ_COMPLEX) = hypot(re, im) + atan2(im, re) * I;       \
}

polar_def1(plus,  +)
polar_def1(minus, -)

#define polar_def2(name, op1, op2)                   \
static INSTR(polar_##name) { GWDEBUG_EXE             \
  POP_REG(shred, SZ_COMPLEX);                        \
  const m_complex a = *(m_complex*)REG(-SZ_COMPLEX); \
  const m_complex b = *(m_complex*)REG(0);           \
  const m_float mag   = creal(a) op1 creal(b);       \
  const m_float phase = cimag(a) op2 cimag(b);       \
  *(m_complex*)REG(-SZ_COMPLEX) = mag  + phase * I;  \
}
polar_def2(mul, *, +)
polar_def2(div, /, -)

#define polar_def1_r(name, op)                                             \
static INSTR(polar_##name##_r) { GWDEBUG_EXE                               \
  POP_REG(shred, SZ_INT);                                                  \
  const m_complex a = *(m_complex*)REG(-SZ_COMPLEX);                       \
  const m_complex b = **(m_complex**)REG(0);                               \
  const m_float re = creal(a) * cos(cimag(a)) op creal(b) * cos(cimag(b)); \
  const m_float im = creal(a) * sin(cimag(a)) op creal(b) * sin(cimag(b)); \
  *(m_complex*)REG(-SZ_COMPLEX) = **(m_complex**)REG(0) =                  \
    hypot(re, im) + atan2(im, re) * I;                                     \
}
polar_def1_r(plus, +)
polar_def1_r(minus, -)

#define polar_def2_r(name, op1, op2)                      \
static INSTR(polar_##name##_r) { GWDEBUG_EXE              \
  POP_REG(shred, SZ_INT);                                 \
  const m_complex a = *(m_complex*)REG(-SZ_COMPLEX);      \
  const m_complex b = **(m_complex**)REG(0);              \
  const m_float mag   = creal(a) op1 creal(b);            \
  const m_float phase = cimag(a) op2 cimag(b);            \
  *(m_complex*)REG(-SZ_COMPLEX) = **(m_complex**)REG(0) = \
    mag + phase * I;                                      \
}
polar_def2_r(mul, *, +)
polar_def2_r(div, /, -)

GWION_IMPORT(complex) {
  CHECK_BB(gwi_class_ini(gwi,  t_complex, NULL, NULL))
	gwi_item_ini(gwi, "float", "re");
  CHECK_BB(gwi_item_end(gwi,   ae_flag_member, NULL))
	gwi_item_ini(gwi, "float", "im");
  CHECK_BB(gwi_item_end(gwi,   ae_flag_member, NULL))
  CHECK_BB(gwi_class_end(gwi))
  CHECK_BB(gwi_class_ini(gwi,  t_polar, NULL, NULL))
  CHECK_BB(gwi_item_ini(gwi, "float", "mod"))
  CHECK_BB(gwi_item_end(gwi,   ae_flag_member, NULL))
  CHECK_BB(gwi_item_ini(gwi, "float", "phase"))
  CHECK_BB(gwi_item_end(gwi,   ae_flag_member, NULL))
  CHECK_BB(gwi_class_end(gwi))
  CHECK_BB(gwi_oper_ini(gwi, "complex", "complex", "complex"))
  CHECK_BB(gwi_oper_add(gwi, opck_assign))
  CHECK_BB(gwi_oper_end(gwi, op_assign,        complex_assign))
  CHECK_BB(gwi_oper_end(gwi, op_add,          complex_plus))
  CHECK_BB(gwi_oper_end(gwi, op_sub,         complex_minus))
  CHECK_BB(gwi_oper_end(gwi, op_mul,         complex_mul))
  CHECK_BB(gwi_oper_end(gwi, op_div,        complex_div))
  CHECK_BB(gwi_oper_add(gwi, opck_rassign))
  CHECK_BB(gwi_oper_end(gwi, op_chuck,         complex_r_assign))
  CHECK_BB(gwi_oper_add(gwi, opck_rassign))
  CHECK_BB(gwi_oper_end(gwi, op_radd,    complex_r_plus))
  CHECK_BB(gwi_oper_add(gwi, opck_rassign))
  CHECK_BB(gwi_oper_end(gwi, op_rsub,   complex_r_minus))
  CHECK_BB(gwi_oper_add(gwi, opck_rassign))
  CHECK_BB(gwi_oper_end(gwi, op_rmul,   complex_r_mul))
  CHECK_BB(gwi_oper_add(gwi, opck_rassign))
  CHECK_BB(gwi_oper_end(gwi, op_rdiv,  complex_r_div))
  CHECK_BB(gwi_oper_ini(gwi, "polar", "polar", "polar"))
  CHECK_BB(gwi_oper_add(gwi, opck_assign))
  CHECK_BB(gwi_oper_end(gwi, op_assign,        complex_assign))
  CHECK_BB(gwi_oper_add(gwi, opck_rassign))
  CHECK_BB(gwi_oper_end(gwi, op_chuck,         complex_r_assign))
  CHECK_BB(gwi_oper_end(gwi, op_add,          polar_plus))
  CHECK_BB(gwi_oper_end(gwi, op_sub,         polar_minus))
  CHECK_BB(gwi_oper_end(gwi, op_mul,         polar_mul))
  CHECK_BB(gwi_oper_end(gwi, op_div,        polar_div))
  CHECK_BB(gwi_oper_add(gwi, opck_rassign))
  CHECK_BB(gwi_oper_end(gwi, op_radd,    polar_plus_r))
  CHECK_BB(gwi_oper_add(gwi, opck_rassign))
  CHECK_BB(gwi_oper_end(gwi, op_rsub,   polar_minus_r))
  CHECK_BB(gwi_oper_add(gwi, opck_rassign))
  CHECK_BB(gwi_oper_end(gwi, op_rmul,   polar_mul_r))
  CHECK_BB(gwi_oper_add(gwi, opck_rassign))
  CHECK_BB(gwi_oper_end(gwi, op_rdiv,  polar_div_r))
  return 1;
}

#ifdef JIT
#include "code/complex.h"
#endif
