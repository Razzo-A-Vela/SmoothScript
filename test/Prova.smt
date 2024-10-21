

func main : <4> {
  return 0;
}

/*
$type void <>;
$type int <4>;

$extern func malloc(int) : *void;         // $extern function from C standard library
$extern func free(*void) : void;
$extern func getTime : int;               // This function does not exist, but let's imagine that it returns the time in milliseconds
$extern $noReturn func exit(int) : void;  // $noReturn tells the parser that this function will end the program before returning
$noReturn func exit : void { exit(0); }

$namespace std {                          // namespaces are now Parsed and not PreProcessed
  $op(+, 10)                              // $op is now similar to inline, but it has some parameters(symbol, precedence) (TODO)
  func op_add(!int? a, !int? b) : !int {
    %add a, b;
    return a;
  }

  func wait(int? millis) : void {
    int? initialMillis = getTime();
    while (getTime() - initialMillis < millis) {}
  }
}


$namespace Player {                       // $namespace used similarly to a class
  $type instanceStruct $struct {
    int posX;
    int posY;
  };
  $type instance *!instanceStruct;

  func move(instance? inst, int? toAddX, int? toAddY) {
    *(inst.posX) += toAddX;
    *(inst.posY) += toAddY;
  }

  func init(instance? toInit) : void {
    (*toInit).posX = 0;
    (*toInit).posY = 0;
  }

  func new : instance {
    instance? ret = (instance) malloc($sizeof(instanceStruct));
    init(ret);
    return ret;
  }

  func free(instance? inst) : void {
    free(inst);
  }
};

!Player::instance? player;

$inline func init : void {
  player = Player::new();
  Player::move(player, 100, 200);
}

func main : int {                         // "main" is always the entry point of the program
  init();
  Player::move(player, 100, 0);
  std::wait(1000);
  Player::move(player, 0, 100);
  std::wait(1000);
  Player::free(player);
  return 0;
}

///////////////////////////////////////////////////////////////

// NEW TYPE SYNTAX BETTER FOR PARSER ($parser)

//! AUTO CALCULATE LEAF AND NON LEAF FUNCTIONS (See Windows (Prolog and Epilog && Calling convention && Software convetion))
//! WITH OPTIMIZATIONS ENABLED SHOULD INLINE LEAF FUNCTIONS
//TODO: CUSTOM ASSEMBLY
//TODO: PREPROCESSOR
//TODO: PARSER (WITH PARSER EXPORT TO GENERATE MULTIPLE TIMES)
//TODO: REDO TOML PARSER (USE PROCESSOR SYSTEM?)
//TODO: GENERATOR (WITH CUSTOM ASSEMBLY GENERATION WITH (TOML FILE || OTHER FILE))

// MUTABLE AND CONSTANT:
// <4>        -> constant 4 bytes (OPTIMIZATION: preCalculate?)
// <4>!       -> mutable 4 bytes
// *<4>       -> constant pointer to constant 4 bytes
// *!<4>      -> mutable pointer to constant 4 bytes
// *<4>!      -> constant pointer to mutable 4 bytes
// *!*<4>     -> mutable pointer to constant pointer to constant 4 bytes
// **!<4>     -> constant pointer to mutable pointer to constant 4 bytes
// **!<>      -> constant pointer to mutable pointer to void (unknown)


#define VOID <>#                          // types as defines (simplest but with no type checking)
                                          // should create functions with different names depending on types
#define BYTE <1>#                         // should create functions for operations with different names depending on types
#define CHAR <1>#
#define SHORT <2>#
#define INT <4>#
#define LONG <8>#

#define FLOAT <f>#
#define DOUBLE <d>#


$type void <>;
                                          // $type <identifier> <byte_type or ($struct or $union)>
$type byte <1>;                           // $type is a #define with type checking in functions, variables and casting
$type char <1>;                           // create a type tree to connect types between one-another
$type short <2>;                          // the type tree should be based on byte_types and two-way or one-way conversions
$type int <4>;                            // <f> <---> <4> <---> int;
$type long <8>;                           // <4> <---- <8> <---> long; (should give a warning when casting <8> to <4>)(cannot cast <4> to <8>)

$type float <f>;                          // Float and double types are built-in (because of register(xmm0, xmm1, ...) and operation(movss, addss, ...) differences)
$type double <d>;                         // <f> can be cast to <4>; <d> can be cast to <8>;

$extern func malloc(int) : *!void;

$type EnumTest <4>;
#namespace EnumTest
  
#

$op(+, 10)
func op_add(!int? a, !int? b) : int {     // Exclamation(!) means mutable (mutable can be cast to constant but not the opposite)
  %add a, b;                              // %Assembly instruction with variables [calculate stack offset or use register (See Windows calling convention)]
  return a;
}

$type TwoInts $struct {
  int? a;
  int? b;
};

$type BOH $struct {
  int? type;
  $union {
    int? i;
    <8> eight;
  } u;
};

$inline                                   // ACTUALLY INLINE THE FUNCTION (if the compiler can't inline throw an error)
func fop_add(<f>? f1, <f>? f2) : <f> {
  %addss @xmm0, @xmm1, @xmm0;             // %Assembly instruction with @register
}

int!? global = 2;                         // Mutable global variable
int? globall = 2;                         // Constant global variable

#define toInt(x) ((int) x)#               //TODO: Add defines with parameters

func main : <4> {
  $struct {
    int? a;
    int? b;
  }? twoInts;

  float? f = (float) 2;                   // 2 -> IEEE; f -> @xmm0[0:3]
  <f>? f = 1.2f;                          // 1.2f -> stack
  f = fop_add(f, 2);                      // -> movss xmm0, stack; movss xmm1, 1.2f(1067030938); addss xmm0, xmm1, xmm0

  twoInts.a = toInt(2) + toInt(1);        // Use define with parameter to cast a "<4>" to "int"
  twoInts.b = 3;
  return 0;
}
*/
