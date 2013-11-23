/* shell.c
   Project 8: Loading and Executing
   
   Chris Wilhelm
*/

#define FILE_NAME_SIZE 7
#define LINE_LENGTH    80
#define FILE_SIZE      13312

#define TRUE           1
#define FALSE          0

// Wrappers for the interrupts
void print(char*);
void scan(char*);
void readFile(char*);

void processCmd(char*);
void processType(char*);
void splitCmds(char*, char*, char*);
int strCmp(char*, char*);
void truncFileName(char*, char*);

int main() {
   char cmd[LINE_LENGTH];
   char cmd1[LINE_LENGTH];
   char cmd2[LINE_LENGTH];

   // Will prompt user for a command
   print("Here is the shell:\n\r\0");
   while ( 1 )
   {
      print("SHELL>\0");
      scan(cmd);
      print("\r");
      splitCmds(cmd, cmd1, cmd2);
      processCmd(cmd1, cmd2);
   }
}

void print(char line[LINE_LENGTH]) {
   interrupt(0x21, 0, line, 0, 0);
}

void scan(char line[LINE_LENGTH]) {
   interrupt(0x21, 1, line, 0, 0);
}

void readFile(char fileName[FILE_NAME_SIZE], char buffer[FILE_SIZE]) {
   interrupt(0x21, 3, fileName, buffer, 0);
}

void processCmd(char cmd1[LINE_LENGTH], char cmd2[LINE_LENGTH]) {
   if ( strCmp(cmd1, "type\0") )
   {
      processType(cmd2);
   }
   else
   {
      print("Bad Command :(\n\r\0");
   }
}

void processType(char fileName[LINE_LENGTH]) {
   char buffer[FILE_SIZE];
   char fileNameTrunc[FILE_NAME_SIZE];

   truncFileName(fileName, fileNameTrunc);
   readFile(fileNameTrunc, buffer);
   print(buffer);
}

void splitCmds(char cmd[LINE_LENGTH], char cmd1[LINE_LENGTH], char cmd2[LINE_LENGTH]) {
   int i = 0;
   int j = 0;

   while ( cmd[i] != ' ' && cmd[i] != 0x0A )
   {
      cmd1[i] = cmd[i];
      i++;
   }
   cmd1[i] = '\0';
   i++;
   while ( cmd[i] != ' ' && cmd[i] != 0x0A )
   {
      cmd2[j] = cmd[i];
      i++;
      j++;
   }
   cmd2[i] = '\0';
}

int strCmp(char str1[LINE_LENGTH], char str2[LINE_LENGTH]) {
   int i = 0;

   while ( str1[i] == str2[i] )
   {
      if ( str1[i] == '\0' && str2[i] == '\0')
      {
         return(TRUE);
      }
      else if ( str1[i] == '\0' || str2[i] == '\0' )
      {
         return(FALSE);
      }
      i++;
   }
   return(FALSE);
}

void truncFileName(char line[LINE_LENGTH], char fileName[FILE_NAME_SIZE]) {
   int i = 0;

   while ( line[i] != '\0' && i < FILE_NAME_SIZE - 1 )
   {
      fileName[i] = line[i];
      i++;
   }
   fileName[i] = '\0';
}