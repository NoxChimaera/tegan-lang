# Non-recursive fibbonacci numbers
func main : int {
  int n = 10

  int x = 1
  int y = 0
  int acc = 0
  int i = 1
  while i <= n {
    acc = x + y
    x = y
    y = acc
    i = i + 1
  }
  print acc
  return 0
}
