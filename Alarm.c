#include<stdio.h>

#include<string.h>

#include <time.h>

#include <termios.h>

#include <unistd.h>

#include <fcntl.h>

#include <unistd.h>

struct alarm {
  int id;
  int hour;
  int min;
  char name[35];
}
alarmList[30], ringingAlarm;
int len = 0;
int /*prevIndex = -1,*/ lastHour = -1, lastMin = -1;
void load_alarms();
void display();
void addAlarm();
void run();
void sound();
void delete();

int kbhit(void) {
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, & oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, & newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, & oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if (ch != EOF) {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}

int main() {
  int op = -1;
  ringingAlarm.hour = -1;
  ringingAlarm.min = -1;
  strcpy(ringingAlarm.name, "NON");
  system("clear");
  printf("******************** WELCOME TO ALARM ************************\n");
  load_alarms();
  while (op != 0) {

    display();
    printf("1.Add Alarm\n2.Run\n3.Delete Alarm\n4.Clear Screen\n0.Exit\nEnter Option:");
    scanf("%d", & op);
    system("clear");
    switch (op) {

    case 1:
      addAlarm();
      break;
    case 2:
      run();
      break;
    case 3:
      delete();
      break;
    case 4:
      system("clear");
      break;
    case 0:

      break;
    default:
      printf("Invalid Option. Try Again.");
    }

  }
  return 0;
}

void sound() {
  system("aplay sound.wav &");
  system("clear");

}
// Show Current Time  + Check for alarms 
// With 1 sec delay
void run() {
  int duration = 14, i;
  int d = 0;

  time_t t;
  struct tm tm;
  getchar();
  while (!kbhit()) {
    t = time(NULL);
    tm = * localtime( & t);

    printf("-------------------------------------------\n");
    printf("	TIME : %d:%d:%d\n", tm.tm_hour, tm.tm_min, tm.tm_sec);

    // Start Ringing
    if (d > 0) {
      printf("-------------------------------------------\n");
      if (d % 2 == 0)
        printf("[ *********** %s *********** ]\n", ringingAlarm.name);
      else
        printf("[ ########### ^ ########### ]\n");
      d--;
    } else {

      // Check for alarm
      for (i = 0; i <= len; i++) {
        if (alarmList[i].hour == tm.tm_hour &&
          alarmList[i].min == tm.tm_min) {

          if (lastHour == alarmList[i].hour &&
            lastMin == alarmList[i].min) {
            continue;
          }
//lh = -1 ,lm =-1 
//50:00:04 50:00:43 
//ring 14  
//05:00:15 5:00
//lm=0 
          ringingAlarm = alarmList[i];
          d = duration;
          lastHour = alarmList[i].hour;
          lastMin = alarmList[i].min;
          sound();
          break;
        }
      }
    }
    printf("-------------------------------------------\n");
    printf("Enter any key to get back to menu.\n");
    sleep(1);
    system("clear");
  }

  getchar();

}

void addAlarm() {
  int h, m;
  char name[30] = "_";
  FILE * f;
  f = fopen("list.txt", "a+");

  printf("-------------------------------------------\n");
  printf("ADD ALARMS\n");
  printf("-------------------------------------------\n");
  printf("Enter Hours:");
  scanf("%d", & h);
  printf("Enter Minutes:");
  scanf("%d", & m);
  printf("Enter Name:");
  scanf("%s", & name);

  if (h < 0 || h > 23) {
    printf("[ Hour cannot less than 0 and grater than 23s ]");
    printf("-------------------------------------------\n");
    return;
  } else if (m < 0 || m > 59) {
    printf("[ Minutes cannot be less than 0 or more than 59 ]");
    printf("-------------------------------------------\n");
    return;
  }

  fprintf(f, "%d %d %s\n", h, m, name);
  fclose(f);
  alarmList[len].hour = h;
  alarmList[len].min = m;
  strcpy(alarmList[len].name, name);
  len++;
  printf("[ Alarm Added ]\n");
  printf("-------------------------------------------\n");

}

void display() {
  int i = 0;
  printf("-------------------------------------------\n");
  printf("                YOUR ALARMS\n");
  printf("-------------------------------------------\n");
  printf("Sr.No\tTIME\tNAME\n");
  printf("-------------------------------------------\n");
  if (len == 0) {
    printf("[ No Alarm Found ]\n");
  } else {
    for (i = 0; i < len; i++) {
      printf(" %d\t%d:%d\t%s\n", i + 1, alarmList[i].hour, alarmList[i].min,
        alarmList[i].name);
    }
  }
  printf("-------------------------------------------\n");
}
void delete() {
  int index, i = 0, j = 0;
  struct alarm arr[30];
  FILE * f;

  display();
  printf("(Press 0 to Go Back)\nEnter Sr.No to Delete:");
  scanf("%d", & index);
  index--;

  if (index >= 0 && index < len) {
    // Write to File expect Deleted One 
    f = fopen("list.txt", "w");
    for (i = 0; i < len; i++) {
      if (i == index)
        continue;

      fprintf(f, "%d %d %s\n", alarmList[i].hour, alarmList[i].min, alarmList[i].name);

    }
    fclose(f);
    load_alarms();
    printf("-------------------------------------------\n");
    printf("Alarm Deleted\n");
    printf("-------------------------------------------\n");
  } else if (index == -1) {
    return;
  } else {
    printf("-------------------------------------------\n");
    printf("Invalid Sr.No\n");
    printf("-------------------------------------------\n");
  }

}

void load_alarms() {
  FILE * filePointer;
  filePointer = fopen("list.txt", "r");
  int index = 0;//add at perticular loc 
  len = 0;//arraylist
  if (filePointer == NULL) {
    printf("[ No File Found ]\n");
    return;
  } else {
    while (!feof(filePointer)) {

      fscanf(filePointer, "%d", & alarmList[index].hour);
      fscanf(filePointer, "%d", & alarmList[index].min);
      fscanf(filePointer, "%s", alarmList[index].name);
      index++;
    }
  }
  fclose(filePointer);

  len = index-1;

}
