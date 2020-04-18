grammar kqasm;

entry : (instruction ENDL+)* EOF
      ;

instruction : ('CTRL' ctrl+=QBIT)? gate=('X'|'Y'|'Z'|'H'|'S'|'SD'|'T'|'TD'|'U1'|'U2'|'U3') ('(' DOUBLE+ ')')? QBIT # gate
            | 'ALLOC' DIRTY? QBIT   # alloc
            | 'FREE' DIRTY? QBIT    # free
            | 'MEASURE' QBIT BIT    # measure
            | 'LABEL' LABEL         # label
            | 'BR' I64 LABEL LABEL  # branch
            | 'JUMP' LABEL          # jump
            | new_int               # int_instr
            ;

new_int : 'INT' I64 ex=('ZE'|'SE') BIT+  # int_ex
        ;

UINT  : [0-9]+;
QBIT  : 'q'UINT;
BIT   : 'c'UINT;
I64   : 'i'UINT;
DOUBLE: '-'?[0-9]+'.'[0-9]*;
LABEL : '@'[a-zA-Z]+[.0-9a-zA-Z]*;
DIRTY : 'DIRTY';
ENDL  : '\r''\n'?|'\n';
WS    : [ \t]+ -> skip;