/* kernel.c
   Project 8: Loading and Executing
   
   Chris Wilhelm
*/

#define IO_INTERRUPT        0x10
#define SECTOR_RW_INTERRUPT 0x13
#define KEYBOARD_INTERRUPT  0x16

#define AH_OUT_TO_CONSOLE  0x0E

#define ENTER_ASCII        0x0D
#define BKSPC_ASCII        0x08

#define LINE_FEED          0x0A
#define END_OF_STRING      0x00

#define SUCCESS            1
#define FAILURE            0

void printString(char*);
void printChar(char);
void readString(char*);
void readSector(char*, int);
void readFile(char*, char*);
void executeProgram(char*, int);
void terminate();
int getFileSectors(char*, char*, char*);
void setFileSectors(char*, char*);
void handleInterrupt21(int, int, int, int);
int mod(int, int);
int div(int, int);

int main() {
   char line[80];
   char buffer[13312];

   makeInterrupt21();
/*
// Step 1:
   // read file into buffer
   interrupt(0x21, 3, "messag\0", buffer, 0);

   // print out the file
   interrupt(0x21, 0, buffer, 0, 0);
*/
// Step 2:
   interrupt(0x21, 4, "tstpr2\0", 0x2000, 0);

   while(1);
}

void printString(char *string) {
   int i = 0;
   int ax;
   // parse the array until null char is found
   while ( string[i] != '\0' )
   {
      ax = AH_OUT_TO_CONSOLE * 256 + string[i];
      interrupt(IO_INTERRUPT, ax, 0, 0, 0);
      i++;
   }
}

void printChar(char character) {
   int ax = AH_OUT_TO_CONSOLE * 256 + character;
   interrupt(IO_INTERRUPT, ax, 0, 0, 0);
}

void readString(char line[80]) {
   char key;
   int i = 0;

   key = interrupt(KEYBOARD_INTERRUPT, 0, 0, 0, 0);
   while ( key != ENTER_ASCII )
   {
      // Handle backspace key press
      if ( key == BKSPC_ASCII && i > 0 )
      {
         i--;
         printString("-BKSPC-");
      }
      else
      {
         printChar(key);
         line[i] = key;
         i++;
      }
      key = interrupt(KEYBOARD_INTERRUPT, 0, 0, 0, 0);
   }
   key = LINE_FEED;
   printChar(key);
   line[i] = key;
   i++;
   key = END_OF_STRING;
   printChar(key);
   line[i] = key;
}

void readSector(char buffer[512], int sector) {
   int ah = 2; // read sector
   int al = 1; // number of sectors to read
   int ax = ah * 256 + al;

   int ch = div(sector, 36); // track number
   int cl = mod(sector, 18) + 1; // relative sector
   int cx = ch * 256 + cl;

   int dh = mod(div(sector, 18), 2); // head number
   int dl = 0; // device number
   int dx = dh * 256 + dl;

   interrupt(SECTOR_RW_INTERRUPT, ax, buffer, cx, dx);
}

void readFile(char fileName[6], char buffer[13312]) {
   char sectorBuff[512];
   char fileSectors[26];

   readSector(sectorBuff, 0x02);
   if ( getFileSectors(fileName, sectorBuff, fileSectors) == SUCCESS )
   {
      setFileSectors(buffer, fileSectors);
   }
}

void executeProgram(char name[6], int segment) {
   char buffer[13312];
   int i = 0;

   readFile(name, buffer);
   while ( i < 13312)
   {
      putInMemory(segment, i, buffer[i]);
      i++;
   }
   launchProgram(segment);
}

void terminate () {
   while(1);
}

int getFileSectors(char fileName[6], char sectorBuff[512], char fileSectors[26]) {
   int i = 0;
   int j = 0;
   int k = 0;
   
   /* parse sectorBuff for first char in fileName */
   while ( i < 512 )
   {
      /* See if all 6 chars of fileName are in sectorBuff */
      while ( sectorBuff[i + j] == fileName[j] )
      {
         if ( j == 5 )
         {
            j++;
            /* When all of fileName is found, find all sectors mapped to
               that file */
            while ( sectorBuff[i + j + k] != 0x00 )
            {
               fileSectors[k] = sectorBuff[i + j + k];
               k++;
            }
            return SUCCESS;
         }
         j++;
      }
      i++;
   }
   return FAILURE;
}

void setFileSectors(char buffer[13312], char fileSectors[26]) {
   char sectorBuff[512];
   int i = 0;
   int j = 0;

   /* lookup each file sector and place data in buffer */
   while ( fileSectors[i] != 0x00 && i < 26)
   {
      j = 0;
      readSector(sectorBuff, fileSectors[i]);
      while ( j < 512 )
      {
         buffer[i * 512 + j] = sectorBuff[j];
         j++;
      }
      i++;
   }
}

void handleInterrupt21(int ax, int bx, int cx, int dx) {
   switch ( ax )
   {
      case 0: // print string
         printString(bx);
         break;
      case 1: // read string
         readString(bx);
         break;
      case 2: // read sector
         readSector(bx, cx);
         break;
      case 3: // read file
         readFile(bx, cx);
         break;
      case 4: // execute program
         executeProgram(bx, cx);
         break;
      case 5: // terminate
         terminate();
         break;
      default:
         printString("Error: Invalid Command\0");
         break;
   }
}

int mod(int a, int b) {
   while ( a >= b )
   {
      a = a - b;
   }
   return a;
}

int div(int a, int b) {
   int quotient = 0;
   while ( (quotient * b) < a )
   {
      quotient++;
   }
   return quotient - 1;
}