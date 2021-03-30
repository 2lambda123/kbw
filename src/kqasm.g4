/* MIT License
 * 
 * Copyright (c) 2020 Evandro Chagas Ribeiro da Rosa <evandro.crr@posgrad.ufsc.br>
 * Copyright (c) 2020 Rafael de Santiago <r.santiago@ufsc.br>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

grammar kqasm;

entry : (instruction ENDL+)* EOF;

instruction : ctrl? gate_name arg_list? QBIT                        # gate
            | ctrl? 'PLUGIN' ADJ? name=STR qubits_list ARGS          # plugin
            | 'ALLOC' DIRTY? QBIT                                   # alloc
            | 'BR' INT then=LABEL otherwise=LABEL                # branch
            | 'DUMP' qubits_list                                     # dump
            | 'FREE' DIRTY? QBIT                                    # free
            | 'JUMP' LABEL                                           # jump
            | 'LABEL' LABEL                                          # label
            | 'MEASURE' INT qubits_list                          # measure
            | 'SET' target=INT from=INT                      # set
            | 'INT' result=INT left=INT bin_op right=INT # binary_op
            | 'INT' INT SIG? UINT                                # const_int
            ;


ctrl : 'CTRL' qubits_list ',';

qubits_list : '[' QBIT (',' QBIT)* ']';

gate_name : 'X'|'Y'|'Z' |'H'|'S'|'SD'|'T'|'TD'|'U1'|'U2'|'U3'|'RZ'|'RX'|'RY';

arg_list : '(' DOUBLE (',' DOUBLE)* ')';

bin_op : '=='|'!='|'>'|'>='|'<'|'<='|'+'|'-'|'*' |'/'|'<<'|'>>'|'and'|'xor'|'or';

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
