#include "ncurses_display.h"
#include "system.h"

int main() {
  System system;
  // start test
float cpu =   system.Cpu().Utilization();
  // end test


  NCursesDisplay::Display(system);
}