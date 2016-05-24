# hello
func main() : int {
  int i = -2;
  while i <= 2 {
    if i < 0 print -1
    else if i > 0 print 1
    else print 0

    i = i + 1
  }

  i = 0;
  do { print i; i = i + 1 } while i < 0
  i = -1
  while i < 0 { print i; i = i + 1 }

  return 0
}
