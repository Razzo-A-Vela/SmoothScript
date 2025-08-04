
int32: a;
int32: c = 2;

;

func f() -> int32 {
  ;
  { a = 10; }

  int32: b;

  if (1) {
    b = 1;
  }

  if (0);

  if (b = 1) return 2;

  int32: d = b = 3;
  a = 2;
  return b = 22;
  return 10;
}
