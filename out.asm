global main
section .text

main:
  mov rax, 0
  push rax


  mov rax, 4
  push rax

  mov rbx, 2
  pop rax
  sub rax, rbx

  push rax

  mov rax, 10
  push rax

  mov rbx, 2
  pop rax
  mul rbx

  mov rbx, rax
  pop rax
  add rax, rbx

  push rax


  mov rax, 0
  push rax


  mov rax, [rsp + 8]
  push rax

  mov rbx, 22
  pop rax
  cmp rax, rbx
  je bool_bin_expr_true__1
  mov rax, 0
  jmp bool_bin_expr_false__2
bool_bin_expr_true__1:
  mov rax, 1
bool_bin_expr_false__2:

  mov rbx, 0
  cmp rax, rbx
  je end_if__0


  mov rax, [rsp + 16]
  push rax

  mov rbx, 1
  pop rax
  add rax, rbx

  mov [rsp + 16], rax
end_if__0:


while_loop__3:
  mov rax, [rsp + 16]
  push rax

  mov rbx, 3
  pop rax
  cmp rax, rbx
  jne bool_bin_expr_true__5
  mov rax, 0
  jmp bool_bin_expr_false__6
bool_bin_expr_true__5:
  mov rax, 1
bool_bin_expr_false__6:

  mov rbx, 0
  cmp rax, rbx
  je end_while__4


  mov rax, [rsp + 0]
  push rax

  mov rbx, 1
  pop rax
  add rax, rbx

  mov [rsp + 0], rax


  mov rax, [rsp + 16]
  push rax

  mov rbx, 1
  pop rax
  add rax, rbx

  mov [rsp + 16], rax
  jmp while_loop__3
end_while__4:


  mov rax, [rsp + 0]
  pop rbx
  pop rbx
  pop rbx
  ret


  pop rbx
  pop rbx
  pop rbx
  mov rax, 0
  ret
