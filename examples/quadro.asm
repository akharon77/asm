inp
pop rax

inp
pop rbx

inp
pop rcx

cmp RAX 0
je  linear
jne quadro

linear:
    cmp RBX 0

    je  b_zero
    jne b_no_zero

    b_zero:
        cmp rcx 0
        je  infty
        jne no_root

        infty:
            push 1000000000
            out
            
            jmp end

    b_no_zero:
        push 1
        out

        push rcx
        push -1
        mul

        push 100
        mul

        push rbx
        div

        out

        jmp end

quadro:
    push  RBX
    push  RBX
    mul

    push  4
    push  RAX
    mul
    push  RCX
    mul

    sub

    pop  RDX
    cmp  RDX 0

    jb no_root

    push RDX
    push 10000
    mul

    sqrt

    push 100
    div

    pop  RDX

    je one_root
    ja two_roots

    one_root:
        push 1
        out

        push RBX
        push -1
        mul

        push 100
        mul

        push 2
        push RAX
        mul

        div

        out
        
        jmp end

    two_roots:
        push 2
        out

        push RBX
        push -1
        mul
        
        push RDX
        add

        push 100
        mul

        push 2
        push RAX
        mul

        div

        out

        push RBX
        push -1
        mul
        
        push RDX
        sub

        push 100
        mul

        push 2
        push RAX
        mul

        div

        out

        jmp end

    no_root:
        push 0
        out
        jmp end

end:

