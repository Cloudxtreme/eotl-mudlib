/*
** closures.h
**
** Some macros to make closures a bit more readable for LPC dweebs
**
** Originally written by Iffy, adopted by Acme with permission.
**
*/

#ifndef ACME_CLOSURES_INC
#define ACME_CLOSURES_INC


#define C_ASSIGN(a,b)        ({ #'=, a, b })
#define C_IDX(a,i)           ({ #'[, a, i })
#define C_I2DX(a,i,j)        IDX(IDX(a,i),j)
#define C_I3DX(a,i,j,k)      IDX(IDX(IDX(a,i),j),k)
#define C_I4DX(a,i,j,k,l)    IDX(IDX(IDX(IDX(a,i),j),k),l)
#define C_AEQU(a,b)          ({ #'+=, a, b })
#define C_SEQU(a,b)          ({ #'-=, a, b })
#define C_DEQU(a,b)          ({ #'/=, a, b })
#define C_MEQU(a,b)          ({ #'*=, a, b })
#define C_ANDEQU(a,b)        ({ #'&=, a, b })
#define C_OREQU(a,b)         ({ #'|=, a, b })

#define C_LT(a,b)            ({ #'<, a, b })
#define C_LE(a,b)            ({ #'<=, a, b })
#define C_GT(a,b)            ({ #'>, a, b })
#define C_GE(a,b)            ({ #'>=, a, b })
#define C_EQ(a,b)            ({ #'==, a, b })
#define C_NEQ(a,b)           ({ #'!=, a, b })

#define C_LAND(a,b)          ({ #'&&, a, b })
#define C_LOR(a,b)           ({ #'||, a, b })
#define C_BAND(a,b)          ({ #'&, a, b })
#define C_BOR(a,b)           ({ #'|, a, b })
#define C_ADD(a,b)           ({ #'+, a, b })
#define C_SUB(a,b)           ({ #'-, a, b })
#define C_MUL(a,b)           ({ #'*, a, b })
#define C_DIV(a,b)           ({ #'/, a, b })

#define C_IF(t,r)            ({ #'?, t, r })
#define C_IFN(t,r)           ({ #'?!, t, r })
#define C_WHILE(t,b)         ({ #'while, t, 0, b })
#define C_DO(b,t)            ({ #'do, b, t, 0 })

#define C_RANDOM(a)          ({ #'random, a })

#define C_FOR(v,i,t,b)       EQU(v,i), C_WHILE(t,b)

#define C_INC(a)             C_ADD(a,1)
#define C_DEC(a)             C_SUB(a,1)
#define C_CABS(a)            C_IF(C_LT(a, 0), SUB(0, a), a)

#define C_THISP             ({ #'this_player })
#define C_THISO             ({ #'this_object })
#define C_PNAME             ({ #'call_other, C_THISP "query_name" })
#define C_PRNAME            ({ #'call_other, C_THISP "query_real_name" })
#define C_GetLevel( x ) \
        ({ #'?, \
          ({ #'=, \
            'temp, \
            ({ #'+, \
              ({ #'searcha, ({ #'geteuid, x }), ':' }), \
              1 \
              }) \
            }), \
          ({ #'extract, ({ #'geteuid, x }), 'temp }), \
          NOLEV \
          })
#define C_OrdLevel( x )      ({ #'searcha, quote(_WizLevels), x })
#define C_GetOrdLevel( x )   C_OrdLevel( C_GetLevel( x ) )
#define C_IsWizard( x )      ({ #'>=, \
                               C_GetOrdLevel( x ), \
                               C_OrdLevel( Frob ) \
                             })
#endif
