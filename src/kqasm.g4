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

entry : (instruction ENDL+)* EOF
      ;

instruction : ('CTRL' ctrl+=QBIT)? gate=('X'|'Y'|'Z'|'H'|'S'|'SD'|'T'|'TD'|'U1'|'U2'|'U3') ('(' DOUBLE+ ')')? QBIT # gate
            | 'PLUGIN' STR QBIT+ ARGS? # plugin
            | 'ALLOC' DIRTY? QBIT     # alloc
            | 'FREE' DIRTY? QBIT      # free
            | 'MEASURE' QBIT BIT      # measure
            | 'LABEL' LABEL           # label
            | 'BR' I64 LABEL LABEL    # branch
            | 'JUMP' LABEL            # jump
            | new_int                 # int_instr
            | 'DUMP' QBIT             # dump
            ;

new_int : 'INT' I64 ex=('ZE'|'SE') BIT+  # int_ex
        | 'INT' I64 SIG? UINT            # int_const
        | 'INT' I64 I64 op=('=='|'!='|'>'|'>='|'<'|'<='|'+'|'-'|'*'|'/'|'<<'|'>>'|'and'|'xor'|'or') I64 # int_infix
        ;

UINT  : [0-9]+;
SIG   : '-';
QBIT  : 'q'UINT;
BIT   : 'c'UINT;
I64   : 'i'UINT;
DOUBLE: '-'?[0-9]+'.'[0-9]*;
STR   : [a-zA-Z]+[._0-9a-zA-Z]*;
LABEL : '@'STR;
ARGS  : '"'~["]+'"';
DIRTY : 'DIRTY';
ENDL  : '\r''\n'?|'\n';
WS    : [ \t]+ -> skip;
