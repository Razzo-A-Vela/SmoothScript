global main
section .text

main§§4:
  push rbp
  mov rbp, rsp
  
  call a§§4
  mov rsp, rbp
  pop rbp
  ret
  
  mov rsp, rbp
  pop rbp
  ret

a§§4:
  push rbp
  mov rbp, rsp
  
  mov eax, 3
  mov rsp, rbp
  pop rbp
  ret
  
  mov rsp, rbp
  pop rbp
  ret

main:
  push rbp
  mov rbp, rsp

  call main§§4

  mov rsp, rbp
  pop rbp
  ret
