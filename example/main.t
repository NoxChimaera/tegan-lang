# hello
func sign( int i ) : int {
  if i >= 0 {
    if i > 0 { return 1 }
    else { return 0 }
  } else { return -1 }

  return 0

  # int res = 0;
  # if i > 0 res = 1
  # else if i < 0 res = -1
  #
  # return res
}

func main() : int {

  int i = -2;
  while i <= 2  {
    print sign( i )
    i = i + 1
  }

  # int i = -2;
  # while i <= 2 {
  #   print sign( i )
  #   i = i + 1
  # }
  #
  # i = 0;
  # do { print i; i = i + 1 } while i < 0
  # i = -1
  # while i < 0 { print i; i = i + 1 }

  return 0
}
