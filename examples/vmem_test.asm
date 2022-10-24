push 1

loop:
    pop rax
    cmp rax 100

    je end

    push rax
    push 1
    add

    vmout

    jmp loop

end:
