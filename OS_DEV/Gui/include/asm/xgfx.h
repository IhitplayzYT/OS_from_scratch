; xgfx.h
bits 16

%macro arg 2
    %%of equ ((4*%2)+6)
        mov %1,[bp+%%of]
%endmacro
