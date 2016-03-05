import random
import subprocess

success = 0
fail = 0

def toString(line):
  return ''.join([str(x) for x in line])

def make_input(n):
  board = [[random.randint(0, 1) for y in range(n)] for x in range(n)]

  def get_col(col):
    return [board[x][col] for x in range(n)]

  def get_hint(line):
    s = toString(line)
    ones = s.split('0')
    ones = filter(lambda a: a != '', ones)
    if ones == []:
      return [1]
    else:
      return [len(x) for x in ones]

  rowhints = [get_hint(row) for row in board]
  colhints = [get_hint(get_col(col)) for col in range(n)]
  return (board, rowhints + colhints)

def test(t):
  for tt in range(t):
    inputfile = open("inputfile", "w")
    outputfile = open("outputfile", "w")
    (board, hints) = make_input(5)
    inputfile.write("5 5\n")
    for h in hints:
      inputfile.write(str(len(h)))
      inputfile.write(' ')
      for x in h:
        inputfile.write(str(x))
        inputfile.write(' ')
      inputfile.write('\n')
    
    inputfile.close()

    ps = subprocess.Popen(('cat', 'inputfile'), stdout = subprocess.PIPE)
    subprocess.call('./a.out', stdin = ps.stdout, stdout=outputfile)
    ps.wait()
    outputfile.close()
   
    def change(c):
      if c == 'O':
        return '1'
      if c == 'X':
        return '0'
      return c

    readoutput = open("outputfile", "r")
    same = True
    debug = ""
    for i in range(len(board)):
      line = readoutput.readline()
      line = line.strip()
      line = ''.join([change(c) for c in line])
      debug += toString(board[i]) + ' ' + line + '\n'
      if toString(board[i]) != line:
        same = False

    global success
    global fail
    if same:
      success += 1
    else:
      print("fail")
      debug = debug.strip()
      print(debug)
      print(hints)
      fail += 1


if __name__ == "__main__":
  test(100)  
  print("success", success, "fail", fail)
