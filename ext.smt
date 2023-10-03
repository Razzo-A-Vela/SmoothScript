extend ext2.smt as EXT;

print : () {
  put('H');
  put('i');
  put('\n');
}

fib : (n) {
  if (n <= 1) return 1;
  return fib(n-1) + fib(n-2);
}

$main
print();
