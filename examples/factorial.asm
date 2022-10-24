inp
pop rax

push 1
pop rbx

push 0
pop rcx

loop:
    push rcx
    push 1
    add
    pop rcx

    push rbx
    push rcx
    mul
    pop rbx
    
    cmp rcx rax
    je end
    jmp loop

end:

push rbx
out
