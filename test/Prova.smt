
/*
//TODO: Custom assembly
func op_add(<4>!? a, <4>!? b) : <4> {     // Exclamation(!) means constant (All parameters MUST be constant)
  %push @rbx;                             // %Assembly instruction with @register
  %mov @rax, a;                           // %Assembly instruction with @register and variable [calculate stack offset or use register (See Windows calling convention)]
  %mov @rbx, b;
  %add @rax, @rbx;
  %pop @rbx;
}

<4>!?? b = 2;                             // Constant global variable
*/

#define INT <4>#

#namespace A
  INT!?? a = 2;

  #namespace B
    INT!?? a = a + 2;
  #

  INT!?? c = B::a + a;
#

func main : INT {
  return A::c;
}
