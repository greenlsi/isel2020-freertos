#include <stdio.h>

void
printxy (int x, int y, const char* txt)
{
  printf ("\e7\e[?25l"); // save cursor, hide cursor
  printf ("\e[%d;%df%s", y, x, txt);
  printf ("\e8\e[?25h"); // show cursor, restore cursor position
  fflush (stdout);
}

