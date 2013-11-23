/* shell.c
   Project 8: Loading and Executing
   
   Chris Wilhelm
*/

#define LINE_LENGTH 80

// Wrappers for the interrupts
void print(char*);
void scan(char*);

void processCmd(char*);

int main() {
   char cmd[LINE_LENGTH];
   
   // Will prompt user for a command
   print("Here is the shell:\n\r\0");
   while ( 1 )
   {
      print("SHELL>\0");
      scan(cmd);
      print("\r");
      processCmd(cmd);
   }
}

void print(char line[LINE_LENGTH]) {
   interrupt(0x21, 0, line, 0, 0);
}

void scan(char line[LINE_LENGTH]) {
   interrupt(0x21, 1, line, 0, 0);
}

void processCmd(char cmd[LINE_LENGTH]) {
   print("Bad Command :(\n\r\0");
}