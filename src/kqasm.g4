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

instruction : ctrl? gate=('X'|'Y'|'Z'
                         |'H'|'S'|'SD'
                         |'T'|'TD'|'U1'
                         |'U2'|'U3'|'RZ'
                         |'RX'|'RY' ) ('(' DOUBLE+ ')')? QBIT     # gate
            | ctrl? 'PLUGIN' ADJ? STR QBIT+ ARGS                  # plugin
            | 'ALLOC' DIRTY? QBIT                                 # alloc
            | 'BR' I64 then=LABEL otherwise=LABEL                 # branch
            | 'DUMP' QBIT+                                        # dump
            | 'FREE' DIRTY? QBIT                                  # free
            | 'JUMP' LABEL                                        # jump
            | 'LABEL' LABEL                                       # label
            | 'MEASURE' QBIT                                      # measure
            | 'SET' target=I64 from=I64                           # set
            | new_int                                             # int_instr
            ;

ctrl : 'CTRL' QBIT+;

new_int : 'INT' result=I64 left=I64 op=('=='|'!='|'>'
                                       |'>='|'<'|'<='
                                       |'+'|'-'|'*'
                                       |'/'|'<<'|'>>'
                                       |'and'|'xor'|'or') right=I64 # int_infix
        | 'INT' I64 ex=('ZE'|'SE') BIT+                             # int_ex
        | 'INT' I64 SIG? UINT                                       # int_const
        ;

ADJ   : '!';
ARGS  : '"'~["]+'"';
BIT   : 'c'UINT;
DIRTY : 'DIRTY';
DOUBLE: '-'?[0-9]+'.'[0-9]*;
ENDL  : '\r''\n'?|'\n';
I64   : 'i'UINT;
LABEL : '@'STR;
QBIT  : 'q'UINT;
SIG   : '-';
STR   : [a-zA-Z]+[._0-9a-zA-Z]*;
UINT  : [0-9]+;
WS    : [ \t]+ -> skip;
