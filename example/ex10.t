func sign( int a ) : int {
  int res = 0
  if a > 0 res = 1
  else if a < 0 res = -1
  return res
}

func main() : int {

  int i = -2;
  while i <= 2  {
    print sign( i )
    i = i + 1
  }

  return 0
}
