#include "english_time.h"
#include "string.h"

static const char* const ONETEENS[] = {
  "zero",
  "one",
  "two",
  "three",
  "four",
  "five",
  "six",
  "seven",
  "eight",
  "nine",
  "ten",
  "eleven",
  "twelve",
  "thirteen",
  "four\nteen",
  "fifteen",
  "sixteen",
  "seven\nteen",
  "eight\nteen",
  "nine\nteen"
};

static const char* const TWENS[] = {
  "twenty",
  "thirty",
  "forty",
  "fifty",
};


static const char* STR_OH_CLOCK = "o'clock";
static const char* STR_OH = "o'";

void english_time(int hours, int minutes, char* str_hour, char* str_minute) {
    
  strcpy(str_hour, "");
  strcpy(str_minute, "");

  if(hours == 0) {
    strcat(str_hour,"twelve");
  }
  else {
    if (hours > 12) {
      hours = hours - 12;
    }
    strcat(str_hour, ONETEENS[hours]);
  }

  if(minutes == 0){
    strcat(str_minute,STR_OH_CLOCK);
  }
  else {
    if(minutes < 20) {
      if(minutes < 10) {
        strcat(str_minute, STR_OH);  
      }
      strcat(str_minute, ONETEENS[minutes]);
    }
    else {
      strcat(str_minute, TWENS[((minutes/10)%10)-2]);
      if((minutes%10)>0) {
        strcat(str_minute, "\n");
        strcat(str_minute, ONETEENS[minutes%10]); 
      } 
    }
  }
}