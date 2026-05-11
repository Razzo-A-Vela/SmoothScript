
;

:unsigned int b = 2;
:const float c = b-- + 3.2 - .1 + 2.;
:size_t d, i = 3, j;
:const unsigned char e = d = 6;
:char otherChar = 'A';

func h(:int a, b, c :int x) !;

func g() void {
  return;
}

func f() int {
  ;
  { a = 0o12; }

  for (:int i = 0; i < 10; i++) {}

  for (;;)
    break;

  :int b;

  if (1 || !4 && 2) {
    b = 1;
  } else
    p = 0xC;

  while (i > 10 >= 0)
    i++;
  
  do {
    i--;
  } while (i == ~3);

  do
    j++;
  while (j < 0b11);

  do {
    i++;
  } while (i <= 18) {
    i++;
  }

  ::label;

  loop;
  loop {
    if (0);
    else {
      break;
    }

    continue;
  }

  goto label;

  g();

  if (b != 1)
    return 2;

  :cstr str = "Ciao";
  :const int d = (b = h(10, 11, 12, 13));
  (a = 2);
  return b = (22);
  return (10);
}
