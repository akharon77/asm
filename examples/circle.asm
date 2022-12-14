push -1
pop rax

line:
    push rax
    push 1
    add

    pop rax

    cmp rax 24
    je end

    push -1
    pop rbx

    col:
        push rbx
        push 1
        add

        pop rbx

        cmp rbx 80
        je line

        push rax
        push rbx
        call dist

        push 10
        sub

        pop rcx
        cmp rcx 0

        ja col

        push 80
        push rax
        mul
        push rbx
        add

        call dot

        jmp col
end:

vmout

hlt

dist:
    pop [2]
    pop [1]

    push [1]
    push 12
    sub
    push [1]
    push 12
    sub
    mul

    push [2]
    push 35
    sub
    push 2
    div
    push [2]
    push 35
    sub
    push 2
    div
    mul

    add

    push 10000
    mul

    sqrt

    push 100
    div

    ret

dot:
    pop rdx
    push 1
    pop [rdx]
    ret
