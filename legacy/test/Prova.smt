extend ext.smt as ext;
extend ext2.smt;

# print : (str) {
#   for (var i = 0; i < len(str); i++;) {
#     putchar(str[i]);
#   }
#   putchar('\n')
# }

# printArr : (arr) {
#   for (var i = 0; i < len(str); i++) {
#     for (var j = 0; j < len(str); j++) {
#       putchar('0' + arr[i] / j);
#     }
#     putchar('\n');
#   }
# }

$main

# TODO: arrays and strings (get the length with len())

# var arr = [ 10, '2' ];
# var str = "daidjjdi";
# printArr(arr);
# print(str);

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
var out = ext.EXT.add(add(z, get()), 2);
put(out);

exit(out);
