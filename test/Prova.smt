
/*
#define A ;#

// Custom assembly
func op_add(<4>?! a, <4>?! b) : <4> {     // Exclamation(!) means constant (All parameters MUST be constant)
  %push @rbx;                             // %Assembly instruction with @register
  %mov @rax, a;                           // %Assembly instruction with @register and variable [calculate stack offset or use register (See Windows calling convention)]
  %mov @rbx, b;
  %add @rax, @rbx;
  %pop @rbx;
}

<4>?? b = 2;
*/

#define INT <4>#

// NAMESPACE PASS (AND NAMESPACE WALKER PASS)
// CONDITIONS: #preprocessor|DOUBLE_QUESTION|func|proc <identifier>;
//                            save identifier to reuse in a recursive? way

#namespace A {                  // Get all identifiers (conditions above)
  #define RET 0#                // RET

  #namespace B {                // Get all identifiers (conditions above)
    INT?? b = RET + 2;          // b; RET?"A:RET"
  }#                            // Apply all identifiers -> B::b

  // Still a global variable because #namespaces are just syntaxtic sugar to organize code
  // Global variable with ?? [MUST not be constant (If you want a global constant variable just use a define)]
  INT?? a = B::b + RET;         // a; B::b?"A:B"::"b"; RET?"A:RET"
}#                              // Apply all identifiers -> A::RET; A::B::b, A::a

//                              // Namespace walker at the end of everything as a different pass

func main : INT {
  /*
  b = op_add(b, 2);             // #ops are just functions with precedence
  <4>?! a = op_add(b, 2);       // Constant variable
  return op_add(a, 10);
  */

  return RET
}
