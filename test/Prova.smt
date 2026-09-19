
using :signed int<8> char;
using :unsigned char byte;
using :int<32> int, float;
using :unsigned int<64> size_t;
using :unsigned int test_t, test2_t;
using :signed int cint;

:test_t mut test;
:test2_t test2;

;

:unsigned int b = -2;
:float c = b-- + 3.2 - .1 + 2.;
:size_t d, mut i = 3, const j;  // for variables, const is optional
:byte mut e = d = 6;
const otherChar := 'A';  // for variables, const is optional

func h(:int a, b, c :int x) !;

func g() void {
  return;
}

func ahOk(:float x, y :int m) float {
  :float ret = 0;
  for (:int i = 0; i < m; i++)
    ret = ret + (x - y + 1.2);
  return ret;
}

func f() signed int {
  ;
  { a.l.k = +0o12; }

  using :int index_t;

  abc := 1;
  mut def := 2;

  for (:index_t mut i = 0; i < 10; i++) {}

  for (;;)
    break;

  :int mut b;
  :float const m = ahOk().h;

  if (1 || !4 && 2) {
    b = 1;
  } else
    p = 0xC;

  while (i > 10 >= 0)
    ++i;
  
  do {
    i--;
  } while (i == ~3);

  do
    j++;
  while (j < 0b11);

  do {
    --i;
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

  g().h.f.t;

  if (b != 1)
    return 2;

  :cstr const str = "Ciao";
  :int d = (.b = h(10, 11, 12, 13).b().f);
  (a = 2);
  return b = (22);
  return (10);
}
