
print : () {
  put('H');
  put('i');
  put('\n');
}

add : (a, b) {
  return a + b;
}

fib : (n) {
  if (n <= 1) return 1;
  return fib(n-1) + fib(n-2);
}

$main
print();
