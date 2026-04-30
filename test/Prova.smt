
;

:int b = 2;
:int c = b;
:int d, h = 3, j;
:int e = d = 6;

func h() int;

func g() int {}

func f() int {
  ;
  { a = 10; }

  :int b;

  // if (1) {
  //   b = 1;
  // }

  // if (0);

  // if (b = 1) return 2;

  :int d = b = 3;
  a = 2;
  return b = 22;
  return 10;
}
