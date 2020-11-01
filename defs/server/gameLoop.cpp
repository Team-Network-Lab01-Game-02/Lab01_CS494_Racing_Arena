#include "server.h"

bool sessionEnd = 0;
int count = 0;

int gameLoop() {
  cout << "Loading" << endl;
  count++;
  if (count == 10) sessionEnd = 1;
  if (sessionEnd) return 0;
  return gameLoop();
}
