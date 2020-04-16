grammar kqasm;

entry : (instruction ENDL+)* EOF
      ;

instruction : ctrl
            | 'ALLOC' 'DIRTY'? QBIT
            | 'FREE' 'DIRTY'? QBIT
            | 'MEASURE' QBIT BIT
            | 'LABEL' LABEL
            | 'BR' I64 LABEL LABEL
            | 'JUMP' LABEL
            | new_int
            ;

ctrl : ('CTRL' QBIT+)? gate
     ;

gate : 'X'  QBIT
     | 'Y'  QBIT 
     | 'Z'  QBIT
     | 'H'  QBIT
     | 'S'  QBIT
     | 'SD' QBIT  
     | 'T'  QBIT
     | 'TD' QBIT
     ;       

new_int : 'INT' I64 ex=('ZE'|'SE') BIT+
        ;

UINT : [0-9]+;
QBIT : 'q'UINT;
BIT  : 'c'UINT;
I64  : 'i'UINT;
LABEL: '@'[a-zA-Z]+[.0-9a-zA-Z]*;
ENDL : '\r''\n'?|'\n';
WS   : [ \t]+ -> skip;