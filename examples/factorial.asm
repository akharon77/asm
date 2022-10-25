inp
call fact
out

hlt

fact:
    pop rax
    cmp rax 0
    je  ret_1
    jmp ret_rec

    ret_1:
        push 1
        ret

    ret_rec:
        push rax
        push rax
        push 1
        sub
        call fact
        mul
        ret

