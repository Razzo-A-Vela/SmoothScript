
;

:int b = 2;
:int c = b--;
:int d, i = 3, j;
:int e = d = 6;

func h(:int a, b, c :int x) !;

func g() void {
  return;
}

func f() int {
  ;
  { a = 10; }

  :int b;

  if (1 || !4 && 2) {
    b = 1;
  } else
    p = 12;

  while (i > 10 >= 0)
    i++;
  
  do {
    i--;
  } while (i == ~3);

  do
    j++;
  while (j < 3);

  do {
    i++;
  } while (i <= 18) {
    i++;
  }

  loop {
    if (0);
    else {
      break;
    }

    continue;
  }

  g();

  if (b != 1)
    return 2;

  :int d = (b = h(10, 11, 12, 13));
  (a = 2);
  return b = (22);
  return (10);
}
