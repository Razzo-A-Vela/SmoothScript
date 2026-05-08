
;

:int b = 2;
:int c = b;
:int d, h = 3, j;
:int e = d = 6;

func h(:int a, b, c :int x) !;

func g() void {}

func f() int {
  ;
  { a = 10; }

  :int b;

  if (1) {
    b = 1;
  }

  if (0);

  g();

  if (b = 1)
    return 2;

  :int d = (b = h(10, 11, 12, 13));
  (a = 2);
  return b = (22);
  return (10);
}
