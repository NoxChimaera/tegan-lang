func sign( int a ) : int {
  if a >= 0
    if a > 0 return 1
    else return 0
  else return -1
  return 0
}

func main() : int {

  int i = -2;
  while i <= 2  {
    print sign( i )
    i = i + 1
  }

  return 0
}
