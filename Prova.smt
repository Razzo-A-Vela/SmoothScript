extend ext.smt as ext;

add : (a, b) {
  return a + b;
}

fib : (n) {
  if (n <= 1) return 1;
  return fib(n-1) + fib(n-2);
}

$main

# Comment
var x;

# y = 22;
var y = (4 - 2) + 10 * 2;
var z;

if (y == 22) x++;

for (var i = 0; i != 10; i++;) 
  if (i == 5)
    break;

# i = 5
z = i;
# x = 1
while (x != 3) {
  z++;
  x++;
}

# z = 7
z = fib(z) - 11;

# z = 10
var out = add(z, get());
put(out);

exit(out);
