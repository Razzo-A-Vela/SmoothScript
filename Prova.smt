extend ext.smt as ext;

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
z = ext.fib(z) - 11;

# z = 10
var out = ext.add(z, get());
put(out);

exit(out);
