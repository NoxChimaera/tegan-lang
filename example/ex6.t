# If-Else statement
func sign( int a ) : int {
  int res = 0
  if a > 0 res = 1
  else if a < 0 res = -1
  return res
}

func main : int {
  print sign( +10 )
  return 0
}
