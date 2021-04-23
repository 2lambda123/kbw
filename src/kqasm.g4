/* Copyright 2020, 2021 Evandro Chagas Ribeiro da Rosa <evandro.crr@posgrad.ufsc.br>
 * Copyright 2020, 2021 Rafael de Santiago <r.santiago@ufsc.br>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

grammar kqasm;

start : block* end_block;

block : label (instruction ENDL+)* end_instruction ENDL+;

end_block : label (instruction ENDL+)*;

label : 'LABEL' LABEL ENDL+;

instruction : ctrl? gate_name arg_list? QBIT                # gate
            | ctrl? 'PLUGIN' ADJ? name=STR qubits_list ARGS # plugin
            | 'ALLOC' DIRTY? QBIT                           # alloc
            | 'FREE' DIRTY? QBIT                            # free
            | 'INT' result=INT left=INT bin_op right=INT    # binary_op
            | 'INT' INT SIG? UINT                           # const_int
            | 'SET' target=INT from=INT                     # set
            | 'MEASURE' INT qubits_list                     # measure
            | 'DUMP' qubits_list                            # dump
            ;

end_instruction : 'BR' INT then=LABEL otherwise=LABEL       # branch
                | 'JUMP' LABEL                              # jump
                ;

ctrl : 'CTRL' qubits_list ',';

qubits_list : '[' QBIT (',' QBIT)* ']';

gate_name : 'X'|'Y'|'Z' |'H'|'S'|'SD'|'T'|'TD'|'U1'|'U2'|'U3'|'RZ'|'RX'|'RY';

arg_list : '(' DOUBLE (',' DOUBLE)* ')';

bin_op : '=='|'!='|'>'|'>='|'<'|'<='|'+'|'-'|'*'|'/'|'<<'|'>>'|'and'|'xor'|'or';

ADJ   : '!';
ARGS  : '"'~["]+'"';
DIRTY : 'DIRTY';
UINT  : [0-9]+;
QBIT  : 'q'UINT;
INT   : 'i'UINT;
DOUBLE: '-'?[0-9]+'.'[0-9]*;
ENDL  : '\r''\n'?|'\n';
LABEL : '@'STR;
SIG   : '-';
STR   : [a-zA-Z]+[._0-9a-zA-Z]*;
WS    : [ \t]+ -> skip;
