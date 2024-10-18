

func main : <4> {
  return 0;
}


/*
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


#define void <>#
#type int <4>#
#op op_add(int, int) : int + 10#

func op_add(int? a, int? b) : int {
  %push @rbx;
  %mov @rax, a;
  %mov @rbx, b;
  %add @rax, @rbx;
  %pop @rbx;
  %ret @rax;
}

//! FUNCTION PARAMETERS MUST NOT BE CONSTANT (OPTIMIZE THE USE OF REGISTERS AND STACK (See Windows calling convention))
//! AUTO CALCULATE LEAF AND NON LEAF FUNCTIONS (See Windows (Prolog and Epilog && Calling convention && Software convetion))
//TODO: FINISH PREPROCESSOR
//TODO: PARSER (WITH PARSER EXPORT TO GENERATE MULTIPLE TIMES)
//TODO: REDO TOML PARSER (USE PROCESSOR SYSTEM?)
//TODO: GENERATOR (WITH CUSTOM ASSEMBLY GENERATION WITH (TOML FILE || OTHER FILE))

#extern func malloc(int?) : *void;

#namespace Player
  #type instance #struct
    int? posX;
    int? posY;
    int? velX;
    int? velY;
  #

  func new : *instance {
    *instance inst = (*instance) malloc(#sizeof instance#);
    init(inst);
    return inst;
  }

  func init(*instance? inst) : void {
    inst->posX = 0;
    inst->posY = 0;
    inst->velX = 0;
    inst->velY = 0;
  }
#

func main : INT {
  Player::instance? player;
  Player::init(&player);
  player.posX += 2; // |-> player.posX = player.posX + 2; |-> player.posX = op_add(player.posX, 2);
  return player.posX;
}
*/
