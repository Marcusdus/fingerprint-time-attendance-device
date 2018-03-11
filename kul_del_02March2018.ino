
#include <EEPROM.h>
//#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
//LiquidCrystal lcd(31,33,35,37,39,41,43,45,47,49);//lcd in 8 bit mode


//#include <Wire.h>
#include "RTClib.h"//for rtc clock
RTC_DS3231 rtc;
#include <avr/pgmspace.h>

#include <SD.h>
#include <Keypad.h>
//FOR LECTURER AND STUDENT
#include <Adafruit_Fingerprint.h>
#include <LiquidCrystal_PCF8574.h>

#define chipSelect 53
LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display

const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
//defining the keypad symbols
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D',}
};
byte rowPins[ROWS] = {22,24,26,28}; //connect to the row pinouts of the kpd
byte colPins[COLS]= {30,32,34,36}; //connect to the column pinouts of the kpd

//initialize an instance of class NewKeypad
Keypad keypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 





//fingerprint to serial communication
#include <SoftwareSerial.h>
SoftwareSerial mySerial(10,11);//Tx,Rx pins of mega, GREEN,YELLOW



Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void retakerReg();
uint8_t getFingerprintEnroll(uint8_t ,uint8_t ,uint8_t );
uint8_t attendance();
void writingToExcel();
void deleteAllPrints();

uint8_t userCount;

static uint8_t lectCounter=0;//has not to exceed 29 (0-30)
static uint8_t lessonCounter=0;
static uint8_t retakerLessons=0;

static uint8_t studCounter=0;//has not to exceed 29 (0-30)

static uint8_t retakerCounter=0;

uint8_t lectFinger;

uint8_t arrCode[10];//array to keep every input data in its array
char key;

//for fingerprint to eeprom

  int i;//for iteration

 //for the rtc to print the exact day in words
 char daysOfTheWeek[7][12] = {"Sund", "Mond", "Tues", "Wedn", "Thur", "Frid", "Satu"};

    
//creating  file pointer
File myfiles1;
File myfiles2;

//for fingerprint to eeprom
static uint8_t id=0;//takes all the users'fingerprint


static uint8_t lectLoc=0;//lecturer's eeprom position counter
static uint8_t lectLoc1=0;
static uint8_t lectLoc2=10;
static uint8_t lectLoc3=20;

static uint8_t studLoc=30;//students'eeprom position counter
static uint8_t studLoc1=30;
static uint8_t studLoc2=60;
static uint8_t studLoc3=90;

static uint8_t retLoc=120;//retakers'eeprom position counter
static uint8_t retLoc1=120;
static uint8_t retLoc2=150;
static uint8_t retLoc3=180;
static uint8_t retLoc4=190;

//adding EEPROM to hold the fingerprints

//student's records

struct students{
        
     
        PROGMEM uint8_t studID[30];//take the number of students registered consecutively
        PROGMEM double regNo[30];
        PROGMEM int prints[30];//will take the value of the fingerprint scanned
        }stud;

//lecturer's records
struct lecturer{
        // is the max number of students
       
        PROGMEM double lessonID[10];
        PROGMEM double lectCode[10];//consecutive numbers of the registered lectures
        PROGMEM int prints[10];//will take the value of the fingerprint scanned
        //PROGMEM uint8_t lectArray[30];//let the max number of users be 30
        }lect;

//retaker's records

struct retaker{
        
        PROGMEM double lessonID[10];
        PROGMEM int studID[30];//take the number of students registered consecutively
        PROGMEM double regNo[30];
        PROGMEM int prints[30];//will take the value of the fingerprint scanned
        }ret;


void setup() {

 
 //////////////INITIALIZING THE DEVICE WITH ALL THE SAVED DATA.....HIBERNETTING/////////////////
int arrLoc,memLoc;
//****************************WRITING TO LECTURER'S RECORD
//**writing lessonID[10] 0-9
for(memLoc=0;memLoc<10;memLoc++)
{
  for(arrLoc=0;arrLoc<10;arrLoc++)
  {
    lect.lessonID[arrLoc]=EEPROM.read(memLoc);//printing data stored by the user from the EEPROM to array...
  }
}
//**writing lectCode[10] 10-19
for(memLoc=10;memLoc<20;memLoc++)
{
  for(arrLoc=0;arrLoc<10;arrLoc++)
  {
    lect.lectCode[arrLoc]=EEPROM.read(memLoc);
  }
}
//**writing prints   20-29
for(memLoc=20;memLoc<30;memLoc++)
{
  for(arrLoc=0;arrLoc<10;arrLoc++)
  {
    lect.prints[arrLoc]=EEPROM.read(memLoc);
  }
}



//WRITING TO STUDENT RECORDS

//**writing regNo[30]  30-59
for(memLoc=30;memLoc<60;memLoc++)
{
  for(arrLoc=0;arrLoc<30;arrLoc++)
  {
    stud.regNo[arrLoc]=EEPROM.read(memLoc);
  }
}

//*** writing studID[30] from  60-89
for(memLoc=60;memLoc<90;memLoc++)
{
  for(arrLoc=0;arrLoc<30;arrLoc++)
  {
    stud.studID[arrLoc]=EEPROM.read(memLoc);
  }
}

//**writing prints[30] 90-119
for(memLoc=90;memLoc<120;memLoc++)
{
  for(arrLoc=0;arrLoc<30;arrLoc++)
  {
    stud.prints[arrLoc]=EEPROM.read(memLoc);
  }
}




//WRITING TO RETAKER RECORDS

//**writing regNo[30]  120-149
for(memLoc=120;memLoc<150;memLoc++)
{
  for(arrLoc=0;arrLoc<30;arrLoc++)
  {
    ret.regNo[arrLoc]=EEPROM.read(memLoc);
  }
}

//*** writing studID[30] from 150-179
for(memLoc=150;memLoc<180;memLoc++)
{
  for(arrLoc=0;arrLoc<30;arrLoc++)
  {
    ret.studID[arrLoc]=EEPROM.read(memLoc);
  }
}

//*writing lessonID[10] 180-189
for(memLoc=180;memLoc<190;memLoc++)
{
  for(arrLoc=0;arrLoc<10;arrLoc++)
  {
    ret.lessonID[arrLoc]=EEPROM.read(memLoc);
  }
}


//**writing prints[30]  190-219
for(memLoc=190;memLoc<220;memLoc++)
{
  for(arrLoc=0;arrLoc<30;arrLoc++)
  {
    ret.prints[arrLoc]=EEPROM.read(memLoc);
  }
}
////////////////////////////////////////////////////////////////////////////////////////////
 //READ VALUES FROM THE EEPROM TO KNOW WHERE THE DEVICE STOPPED WHILE REGISTERING
  //TO AVOID THE STUD,RECT,RET DATA MIXING.

  id=EEPROM.read(233);//takes the last fingerprint template id registered
  lectCounter=EEPROM.read(250);
  lectLoc=EEPROM.read(220);
  lectLoc1=EEPROM.read(221);
  lectLoc2=EEPROM.read(222);
  lectLoc3=EEPROM.read(223);
  
  studCounter=EEPROM.read(251);
  studLoc=EEPROM.read(224);
  studLoc1=EEPROM.read(225);
  studLoc2=EEPROM.read(226);
  studLoc3=EEPROM.read(227);
  
  retakerCounter=EEPROM.read(252);
  retLoc=EEPROM.read( 228);
  retLoc1=EEPROM.read(229);
  retLoc2=EEPROM.read(230);
  retLoc3=EEPROM.read(231);
  retLoc3=EEPROM.read(232);
  
  //READING ALL DATA SAVED IN THE EEPROM WRITING THEM TO THE ARRAYS
  
  
  //Wire.begin();
  //Wire.beginTransmission(0x27);
  lcd.setBacklight(50);//255
  lcd.begin(20,4);
  //lcd.clear();
  Serial.begin(9600);
  delay(100);

  //set SD card baud rate
    
  //SD.begin(CS pin);
  pinMode(chipSelect,OUTPUT); //declare the CS as an OUTPUT
  //SD.begin(chipSelect);
  
  if(!SD.begin(chipSelect))// 53 is chipselect on arduino mega
  {
    Serial.println("SD card not working");
  }
  else
  Serial.println("SD card  working");
  
  //CHECKING THE RTC MODULE ON THE BOARD
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
   // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  
  // set the data rate for the sensor serial port
  finger.begin(57600);
  //-----------------------FINGERPRINT SCANNER PRESENCE TEST------------------------
  if (finger.verifyPassword()) {
    ////Serial.println("Found fingerprint sensor!");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(F("FINGERPRINT "));
    lcd.setCursor(0,1);
    lcd.print(F(" WORKING"));
    Serial.println(" fingerprint working");

    //if working then empty the fingerprints to restart over
   //finger.emptyDatabase();
    delay(1000);
  } else {
    ////Serial.println("Did not find fingerprint sensor :(");
    lcd.print(F("FINGERPRINT"));
    lcd.print(F(" NOT FOUND!"));
    delay(2000);
    while (1) { delay(1); }
  }
 //---------------------------------------------------------------------------------
 
homeScreen();
}

void loop()
{
  
  
  //REGISTER OR ATTEND
   key = keypad.getKey();
   
    switch(key)
  {
    case '1'://for registration

          regOptions();//REGISTERING OPTIONS
          
         
            break;
      case '2'://for attendance
            lcd.clear();
            lcd.setCursor(2,0);
            //lcd.print(F("no attend"));
            attendance();
            break;
      case 'D'://format device
          deleteAllPrints();
          
          break;
    default: 
          break;  
  } 
}

void homeScreen()
    {
       lcd.clear();
       lcd.setCursor(0,1);  
       lcd.print(F("1.REGISTER"));
       lcd.setCursor(0,2);
       lcd.print(F("2.ATTEND"));

       //PRINTING THE IMMEDIATE TIME ON THE HOMESCREEN
  DateTime now = rtc.now();
  lcd.setCursor(15,0);
  lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);
  lcd.setCursor(18,1);
  lcd.print(now.day(), DEC);
  lcd.setCursor(18,2);
  lcd.print(now.month(), DEC);
  lcd.setCursor(15,3);
  lcd.print(now.year(), DEC);
  
    }

void regOptions(){
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(F("REGISTER AS:"));
  
  lcd.setCursor(0,1);
  lcd.print(F("1.LECTURER"));
  
  lcd.setCursor(0,2);
  lcd.print(F("2.STUDENT"));

  lcd.setCursor(0,3);
  lcd.print(F("3.RETAKER"));

  key ='x';//trick the keypad... to let key be x
          
          while(key=='x'){
            
            key=keypad.getKey();//let it be null
            
            while(key==NULL)//while key will be still NULL
            {
              key=keypad.getKey();//try to find a typed non NULL value
            }
           
   switch(key){
       case '1'://lecturer's registration
        lecturerReg();
    
    
    break;
    case '2'://student registration
        studentReg();
        writingToExcel();
        
      break;
    case '3'://retaker's registration
        retakerReg();
      break;
    default:
       break;
    
  }
          }
}


void lecturerReg()
{
    lcd.clear();
    lcd.setCursor(0,0);
    
    lcd.print(F("lesson code:"));
    lcd.setCursor(0,1);
    lect.lessonID[lessonCounter]=codeInput(1,1);//input codes not more than 10 digits on the second row
    ////Serial.println("lect.lessonID");
    ////Serial.println(lect.lessonID[lessonCounter]);

    
    //if enter is pressed
    lcd.setCursor(0,2);
    lcd.print(F("lecturer's code:"));
    lcd.setCursor(0,3);
    lect.lectCode[lectCounter]=codeInput(3,2);//input lecturer's code on the fourth line
   
    //if enter is pressed again
    
    //FINGERPRINT ENROLL
    getFingerprintEnroll(1,0,0);//(uint8_t lectbit,uint8_t studbit,uint8_t retakerbit)

    
    
    //delay(500);
     // homeScreen();
    
    }

   void studentReg()
  {
    lcd.clear();
    lcd.setCursor(0,0);
    
    lcd.print(F("Reg. No:"));
    lcd.setCursor(0,1);
    stud.regNo[studCounter]=codeInput(1,3);//input codes not more than 10 digits on the second row

     Serial.println(F("checking the sd card"));
    //--------------------store students reg no in the SD Card file-----------------------
   myfiles1=SD.open("RegFile.txt",FILE_WRITE);
   if(myfiles1)//if file well opened
   {
    Serial.println(F("studRegFile well opened"));
    myfiles1.println(stud.regNo[studCounter]);
   }
   else if(!myfiles1)
   {
   Serial.println("file not opening");
   }
    myfiles1.close();
   //------------------------------------------------------------------------------------- 
    //if enter is pressed
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(F("FingerPrint:"));
    //FINGERPRINT ENROLL
    getFingerprintEnroll(0,1,0);//(uint8_t lectbit,uint8_t studbit,uint8_t retakerbit)
    

    
    
    //if enter is pressed again go to home screen
     // enterToHomeScreen();
  }

  void enterToHomeScreen()//check if ENTER and output the input characters 
{

              char   key ='x';//trick the keypad... to let key be x
   
          while(key=='x')
            
            key=keypad.getKey();//let it be null
            
            while(key==NULL)//while key will be still NULL
            {
              key=keypad.getKey();//try to find a typed non NULL value
            }
            if(key=='*')//if ENTER pressed
            {
              homeScreen();
            } 
}



double codeInput(int row,int who )//will return the double codeInput
{
  
  int i;//for  for loop iteration
  char key;//get input character from the keypad
  double temp=0;
  char lastKey;

  i=0;
   while(i<10)//for(i=0;i<10;i++)
  {
    
    lcd.setCursor(i,row);//i for the cols and j for the rows

    char   key ='x';//trick the keypad... to let key be x
          while(key=='x')
            
            key=keypad.getKey();//let it be null
            
            while(key==NULL)//while key will be still NULL
            {
              key=keypad.getKey();//try to find a typed non NULL value
            }

            if(key=='*')//if ENTER IS PRESSED
            {
              
            break;
            }
            //delete key
            if(key=='#')//if DELETE key IS PRESSED
            {
              //if @ lesson code...1
              if(who==1)
              {
                lcd.clear();
              lcd.setCursor(0,0);
              lcd.print(F("lesson code:"));
              lcd.setCursor(0,1);
              }
              //if @lecturer's code....2
              if(who==2)
              {
                lcd.clear();
              lcd.setCursor(0,0);
              lcd.print(F("lesson code:"));
              lcd.setCursor(0,1);

              //PRINT THE LESSON CODE RECENTLY ENTERED BY THE USER
              lcd.print(lect.lessonID[lessonCounter]);
              lcd.setCursor(0,2);
              lcd.print(F("lecturer's code:"));
              lcd.setCursor(0,3);
              }
              //if @ student Reg.No:....3
              if(who==3)
              {
                lcd.clear();
              lcd.setCursor(0,0);
               
               lcd.print(F("Reg. No:"));;
              lcd.setCursor(0,1);
              }
              //if @ retaker lesson code:....4
              if(who==4)
              {
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print(F("lesson code:"));
              }
              //if @ retaker Reg. No:....5
              if(who==5)
              {
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print(F("lesson code:"));
                //PRINT THE LESSON CODE RECENTLY ENTERED BY THE USER
                lcd.setCursor(0,1);
                lcd.print(ret.lessonID[retakerLessons]);
                 lcd.setCursor(0,2);
                lcd.print(F("Reg. No:"));
                
              }
              
              
              temp=temp/10;//return  back one column
              ////Serial.println("new temp value:");
              ////Serial.println(temp);
              if(lastKey=='0' || lastKey=='1' || lastKey=='2'|| lastKey=='3'|| lastKey=='4')
              {
                lcd.print((double)temp,0);
              }

              if(lastKey=='5' || lastKey=='6' || lastKey=='7'|| lastKey=='8'|| lastKey=='9')
              {
                temp=temp-1;//125=12 not 13; 456=45 not 46 AFTER DELETE
                lcd.print((double)temp,0);
              }
              i--;
             continue;
            }
             lastKey=key;//to prevent 1225=123,126=13 while deleting
           lcd.print(key);
            temp=temp*10 +(key-'0');
            
          i++;
            
         
       
  } 
    ////Serial.println("the value in temp:");
           ////Serial.println(temp);
           return temp;
}


//---------------------------FINGER CHECK -------------------------------------------------------
uint8_t attendance()
{
   //------------------------------------SCANNING THE FINGER----------------------------
 uint8_t p=1;
 uint8_t startAttend;
  lcd.clear();
  //block for the time display during attendance
  DateTime now = rtc.now();//fetch the data read from the rtc

    lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);
    lcd.setCursor(10,0);
    lcd.print(now.year(), DEC);
    lcd.print('/');
    lcd.print(now.month(), DEC);
    lcd.print('/');
    lcd.print(now.day(), DEC);
    
    
  lcd.setCursor(0,1);
  lcd.print(F("press finger:"));
  lcd.setCursor(0,2);
 
  while(p!=FINGERPRINT_OK)
  {
    p = finger.getImage();
  }

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)
  return -1;
  //-------------------------------SEARCHING THE CORRESPONDING FINGER TEMPLATE-----------

  p = finger.fingerFastSearch();
  
  if (p != FINGERPRINT_OK)// if finger not found
  {
   //lcd.clear();
   lcd.print(F("Finger Not Found"));
   lcd.setCursor(0,2);
   lcd.print(F("TRY AGAIN"));
   delay(1000);
   homeScreen();
  return -1;
  }
 
  for(int i=0;i<=id;i++)//check from all users fingerprint IDs
  {
    //--------------------check if print belongs to the lecturer-------
    //----------------------if not NO CLASS---------------------------
    Serial.println("fingerID detected:");
    Serial.println(finger.fingerID);
    if(lect.prints[i]==finger.fingerID)// && (stud.prints[i]!=finger.fingerID) && (ret.prints[i]!=finger.fingerID))
    {
      //START ATTENDANCE
      lectFinger=finger.fingerID;//variable to hold the ID of who started attendance
     startAttend=1;//lectFinger scans
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(F("ATTENDANCE STARTED"));
      Serial.println(F("ATTENDANCE STARTED"));
      lcd.setCursor(0,1);

      //WAITING FOR THE STUDENTS AND RETAKERS TO ATTEND OR THE SAME LECTURER TO CLOSE
      
      //infinite loop to allow students/retakers attendance to stop only if the same
      //lecturer pressed the fingerprint again
      
      while(startAttend==1)//if lecturer's finger was once scanned
      {
        //-----------------------------------PRESS THE FINGER----------
        delay(1000);//enough delay to prevents finger multiscanning
        p=1;
 
  BLOCK:
  lcd.clear();

  /////////////////////////////////////////////////////////////////////////////////////
  //DATE AND TIME
  DateTime now = rtc.now();//fetch the data read from the rtc

    
    lcd.print(now.year(), DEC);
    lcd.print('/');
    lcd.print(now.month(), DEC);
    lcd.print('/');
    lcd.print(now.day(), DEC);
    lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);
   ////////////////////////////////////////////////////////////////////////////////////////// 
      lcd.setCursor(0,0);
      lcd.print(F("ATTENDANCE STARTED"));
      lcd.setCursor(0,1);
  lcd.print(F("press finger:"));
  Serial.println(F("press finger:"));
  //lcd.setCursor(0,1);
  
 
  while(p!=FINGERPRINT_OK)
  {
    p = finger.getImage();
  }

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)
  return -1;
   //-----------------------CHECK THE MATCHING FINGER TEMPLATE IN THE DATABASE-----
        
  p = finger.fingerFastSearch();
  
  if (p != FINGERPRINT_OK)// if finger not found
  {
   lcd.clear();
   lcd.print(F("Finger Not Found"));
   Serial.println(F("Finger Not Found"));
   
   lcd.setCursor(0,1);
   lcd.print(F("TRY AGAIN"));
   Serial.println(F("TRY AGAIN"));
   delay(1000);
   //homeScreen();
 // return -1;
 goto BLOCK;//if finger is not known then return to the BLOCK to read the next finger
  }
  //----------------------------------------------------------------------------------
        //---------------IF IS OF STUDENT OR A RETAKER THEN HAS ATTENDED SUCCESSFULLY----
  
  for(int j=0;j<=id;j++)//check from all users fingerprint IDs
  {
    //if the fingerID found belongs to that of student
    if((stud.prints[j]==finger.fingerID)&&(lect.prints[j]!=finger.fingerID))
    {
      Serial.println("student id:");
      Serial.println(stud.prints[j]);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(F("ATTENDANCE STARTED"));
      lcd.setCursor(0,1);
      lcd.print(stud.regNo[j]);
      lcd.setCursor(0,2);
      Serial.println("student attended");
      lcd.print(F("ATTENDED"));
      delay(1000);
      break;//if student's fingerprint found then break
    }
    //if the fingerID found belongs to that of retaker
    else if((ret.prints[j]==finger.fingerID)&&(lect
    .prints[j]!=finger.fingerID))
    {
      Serial.println("retaker id:");
      Serial.println(ret.prints[j]);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(F("ATTENDANCE STARTED"));
      lcd.setCursor(0,1);
      lcd.print(ret.regNo[j]);
      lcd.setCursor(0,2);
      Serial.println("retaker attended");
      lcd.print(F("ATTENDED"));
      delay(1000);
      break;//if retaker's fingerprint found then break
    }

      //IF IS OF THE SAME LECTURER THEN ATTENDANCE CLOSED....break;
      
    else if(ret.prints[j]==finger.fingerID)//if the fingerprint is of one of the lecturer
      {
        
        Serial.println("lecturer id:");
      Serial.println(lect.prints[j]);
     
      lcd.clear();  
      lcd.setCursor(0,1);
      if(ret.prints[j]==lectFinger)//if the fingerprint is of the lecturer who started the attendance
      {
        lcd.print(F("ATTENDANCE CLOSED"));
       //Serial.println(F("ATTENDANCE CLOSED"));
       startAttend=0;
      delay(1000);
      break;
      }
      else// if is the different lecturer
      lcd.print("DENIED!");
      delay(500);
      break;
      }
      
       
      }
      
      }
        if(startAttend==0)
       break;
      }

   
    }


  //IF FINGER NOT BELONG TO A LECTURER
  
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(F("NO CLASS"));
   
    
    delay(1000);
    homeScreen();
 
}





//---------------------------FINGER ENROL-------------------------------------------------------
uint8_t getFingerprintEnroll(uint8_t lectbit,uint8_t studbit,uint8_t retakerbit)
{

  
  int address;//to return the address of the id(either having the lecturerer's address or student address)
  lcd.clear();
  lcd.setCursor(4,0);
  //------------------ANALYSE THE FINGERPRINT FIRST-----------------------------------
  int p = -1;
  lcd.print(F("PLACE FINGER:"));
  delay(1000);

  

  //the loop will stop iteration if FINGERPRINT_OK
  while (p != FINGERPRINT_OK)
  {
    //SCANNING THE FINGER
    p = finger.getImage();
    //delay(5000);
    switch (p)
    {
    case FINGERPRINT_OK:
      //Serial.println("Image taken");
      //lcd.clear();
      ////printId(userCount);
      lcd.setCursor(0,1);
      lcd.print(F("Image taken"));
      break;
    case FINGERPRINT_NOFINGER:
      //Serial.println("No Finger");
      //lcd.clear();
     // //printId(userCount);
       lcd.setCursor(0,1);
      lcd.print(F("No Finger"));
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      //Serial.println("Communication error");
      //lcd.clear();
     // //printId(userCount);
       lcd.setCursor(0,1);
      lcd.print(F("Comm Error"));
      break;
    case FINGERPRINT_IMAGEFAIL:
      //Serial.println("Imaging error");
      //lcd.clear();
     // //printId(userCount);
       lcd.setCursor(0,1);
      lcd.print(F("Imaging Error"));
      break;
    default:
      //Serial.println("Unknown error");
      // lcd.clear();
      ////printId(userCount);
       lcd.setCursor(0,1);
      lcd.print(F("Unknown Error"));
      //break;
    }
  }
  //IF FINGER SCANNING IS SUCCESSFULLY DONE THEN
  
  //CONVERTING THE FINGERPRINT IMAGE
  
  p = finger.image2Tz(1);//put 1st template in slot location 1 for verification
  switch (p) {
    case FINGERPRINT_OK:
      //Serial.println("Image converted");
      //lcd.clear();
      //printId(userCount);
      
       lcd.setCursor(0,1);
      lcd.print(F("Image converted"));
      break;
    case FINGERPRINT_IMAGEMESS:
      //Serial.println("Image too messy");
       //lcd.clear();
       //printId(userCount);
        
      return p;//p is -1
    case FINGERPRINT_PACKETRECIEVEERR:
      //Serial.println("Communication error");
            //lcd.clear();
            //printId(userCount);
             lcd.setCursor(0,1);
      lcd.print(F("Comm Error"));
      return p;
    case FINGERPRINT_FEATUREFAIL:
      //Serial.println("Could not find fingerprint features");
           // lcd.clear();
           //printId(userCount);
             lcd.setCursor(0,1);
      lcd.print(F("Feature Not Found"));
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      //Serial.println("Could not find fingerprint features");
                 // lcd.clear();
                 //printId(userCount);
                  lcd.setCursor(0,1);
      lcd.print(F("Feature Not Found"));
      return p;
    default:
      //Serial.println("Unknown error");
                  //lcd.clear();
                  //printId(userCount);
                  lcd.setCursor(0,1);
      lcd.print(F("Unknown Error"));
      return p;
  }
  //DO A DOUBLE CHECK
  
  //Serial.println("Remove finger");
  //lcd.clear();
  //printId(userCount);
   lcd.setCursor(0,1);
  lcd.print(F("Remove Finger"));
  delay(500);
  p = 0;
  
  //if no finger detected then it will wait till the finger is pressed again
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
//Serial.println("ID "); 
  //////Serial.println(id);
  p = -1;
  //---------------------PLACE THE SAME FINGER--------------------------------------
  //Serial.println("Place same finger again");
   //lcd.clear();
   //printId(userCount);
    lcd.setCursor(0,1);
      lcd.print(F("Place again"));
      //lcd.setCursor(0,1);
      //lcd.print(F("   Again"));
   
   //wait for getting finger to scan
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      //Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      //Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      ////Serial.println("Communication error");
      
    
      break;
    case FINGERPRINT_IMAGEFAIL:
      ////Serial.println("Imaging error");
      
   
      break;
    default:
      ////Serial.println("Unknown error");
      
   
      return;
    }
  }
  // OK success!
  //convert the scanned finger-> (2)
  p = finger.image2Tz(2);//place template in the location 2 for verification
  switch (p) {
    case FINGERPRINT_OK:
      ////Serial.println("Image converted");
     
      break;
    case FINGERPRINT_IMAGEMESS:
      ////Serial.println("Image too messy");
      lcd.print(F("               "));
    lcd.print(F("TRY AGAIN"));
    delay(500);
    homeScreen();
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      ////Serial.println("Communication error");
      lcd.print(F("               "));
    lcd.print(F("TRY AGAIN"));
    delay(500);
    homeScreen();
      return p;
    case FINGERPRINT_FEATUREFAIL:
      ////Serial.println("Could not find fingerprint features");
      lcd.print(F("               "));
    lcd.print(F("TRY AGAIN"));
    enterToHomeScreen();
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      ////Serial.println("Could not find fingerprint features");
      lcd.print(F("               "));
    lcd.print(F("TRY AGAIN"));
    delay(500);
    homeScreen();
      return p;
    default: 
    lcd.print(F("               "));
    lcd.print(F("TRY AGAIN"));
    delay(500);
    homeScreen();
      ////Serial.println("Unknown error");
      return p;
  }
  // OK converted!
  
  //take a two print feature template and create a model
  ////Serial.println("Creating model for #");  
  ////Serial.println(id);
  
  p = finger.createModel();//model created
  if (p == FINGERPRINT_OK) { 
    //////Serial.println("Prints matched!");
    lcd.print(F("matched"));
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    ////Serial.println("Communication error");
    lcd.print(F("               "));
    lcd.print(F("TRY AGAIN"));
    delay(500);
    homeScreen();
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    ////Serial.println("mismatch");
    lcd.print(F("               "));
    lcd.print(F("TRY AGAIN"));
    delay(500);
    homeScreen();
    return p;
  } else {
    ////Serial.println("Unknown error");
    lcd.print(F("               "));
    lcd.print(F("TRY AGAIN"));
    delay(500);
    homeScreen();
    return p;
  }
  ////Serial.println("ID "); 
  ////Serial.println(id);

  //store the model of the id
  p = finger.storeModel(id);//store calculated model for later matching
  if (p == FINGERPRINT_OK) {
    //-------------------------FINGER IS STORED-----------------------------------------
    ////Serial.println("REGISTERED");
   // lcd.clear();
   //printId(userCount);
    lcd.setCursor(0,1);
    lcd.print(F("REGISTERED!"));
    

    
    
    
  } 
  else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    ////Serial.println("Communication error");
    lcd.print(F("               "));
    lcd.print(F("TRY AGAIN"));
    
    return p;
  }
  else if (p == FINGERPRINT_BADLOCATION) {
    ////Serial.println("Could not store in that location");
    lcd.print(F("               "));
    lcd.print(F("TRY AGAIN"));
    
    return p;
  } 
  else if (p == FINGERPRINT_FLASHERR) {
    ////Serial.println("Error writing to flash");
    
    return p;
  }
  else {
    ////Serial.println("Unknown error");
    lcd.print(F("               "));
    lcd.print(F("TRY AGAIN"));
    
    return p;
  }
 //--------------------WHOSE FINGERPRINT IS THIS? LECTURER OR STUDENT---------------------
  //if enrolling a lecturer...
  if(lectbit==1 && studbit==0 && retakerbit==0)
  {
    
   // lect.lectArray[lectCounter]=lectCounter;
    ////Serial.println("lecturer's number");
    ////Serial.println("lectCounter and lectArray[lectCounter] must be equal");
    Serial.println("lectCounter:");
    Serial.println(lectCounter);
    ////Serial.println("lectArray[lectCounter]:");
    ////Serial.println(lect.lectArray[lectCounter]);
    
    lect.prints[lectCounter]=id;//save the finger//printId of the lecturer
    Serial.println("lect.prints[lectCounter] equals the number of id");
    Serial.println("id:");
    Serial.println(id);
    Serial.println("lect.prints[lectCounter]:");
    Serial.println(lect.prints[lectCounter]);
     //return to the homeScreen
    
    //  break;

 //-----------------------------WRITING THE LECTURER'S DETAILS TO EEPROM------------------------

address=lectLoc;
for( lectLoc;lectLoc<30;lectLoc++)//30 locations for 10 lecturers
{
  //UPDATE lectLoc VARIABLE IN THE EEPROM LOCATION
    EEPROM.update(220,lectLoc);
  /*
  if(lectLoc>=0 && lectLoc<10)//10 locations for the 10 lecturers' fingerprints
  {
    //id or fingerprint detection
    EEPROM.write(lectLoc,id);//(address,value)
  }
 */
 
 for(lectLoc1=0;lectLoc1<10;lectLoc1++)//10 locations for the 10 lecturers' lessons codes
//(lectLoc>=0 && lectLoc<10)//10 locations for the 10 lecturers' lessons codes
  {
    //UPDATE lectLoc1 VARIABLE IN THE EEPROM LOCATION
    EEPROM.update(221,lectLoc1);
    
    //lecturers code
    EEPROM.put(lectLoc1,lect.lectCode[lectCounter]);//for lectCode[lectCounter] > 255
    break;
  }
  
  for(lectLoc2=10;lectLoc2<20;lectLoc2++)//if(lectLoc>=10 && lectLoc<20)//10 locations for the 10 lecturers' lessons ID
  {
    //UPDATE lectLoc2 VARIABLE IN THE EEPROM LOCATION
    EEPROM.update(222,lectLoc2);
    
    //lesson ID
    EEPROM.put(lectLoc2,lect.lessonID[lectCounter]);//for lessonCode[lectCounter] > 255
    break;
  }
  //10 locations for the lecturers fingerprint templates
  for(lectLoc3=20;lectLoc3<30;lectLoc3++)
  {
    //UPDATE lectLoc2 VARIABLE IN THE EEPROM LOCATION
    EEPROM.update(223,lectLoc3);
    
    //lesson ID
    EEPROM.put(lectLoc3,lect.prints[lectCounter]);//for lessonCode[lectCounter] > 255
    break;
  }

 
  break;//to keep only one value at once
}
 //--------------------------------------------------------------------------------------------

 lectCounter++;
    //UPDATE lectCounter VARIABLE IN THE EEPROM ADDRESS
    EEPROM.update(250,lectCounter);
   
    userCount=lectCounter;
    
    
    id++;//increase the number of overall registered users(lecturer,student,retaker)
    EEPROM.update(233,id);//save to EEPROM
   
  }
//if enrolling a student...
  if(lectbit==0 && studbit==1 && retakerbit==0)
  {
    stud.studID[studCounter]=studCounter;
    stud.prints[studCounter]=id;//save the finger//printId of a student
    
     

 //---------------------------WRITING THE STUDENTS' DETAILS TO EEPROM------------------

  address=studLoc;
  for(studLoc;studLoc<120;studLoc++)//90 locations for 30 students(30 to 119)
{
  //UPDATE studLoc VARIABLE IN THE EEPROM LOCATION
    EEPROM.update(224,studLoc);
    
  //writing the student's details to EEPROM
                    //registration number
     //if(studLoc>=30 && lectLoc<60)//30 locations for registration numbers
      
      for(studLoc1=30;studLoc1<60;studLoc1++)
  {
    //UPDATE studLoc1 VARIABLE IN THE EEPROM LOCATION
    EEPROM.update(225,studLoc1);
    
    //student registration number
    EEPROM.put(studLoc1,stud.regNo[studCounter]);//for regNo[studCounter] > 255
    break;
  }

  //student id
   //if(studLoc>=60 && lectLoc<90)//30 locations for student counter
   for(studLoc2=60;studLoc2<90;studLoc2++)
  {
    //UPDATE studLoc2 VARIABLE IN THE EEPROM LOCATION
    EEPROM.update(226,studLoc2);
    //student numbers
    EEPROM.write(studLoc2,stud.studID[studCounter]);//for studID[studCounter]
    break;
  }
  //saving the student fingerprints templates in EEPROM
  for(studLoc3=90;studLoc2<119;studLoc2++)
  {
    //UPDATE studLoc2 VARIABLE IN THE EEPROM LOCATION
    EEPROM.update(227,studLoc3);
    //student numbers
    EEPROM.write(studLoc3,stud.prints[studCounter]);//for studID[studCounter]
    break;
  }
/*
  //student fingerprint id
   if(studLoc>=90 && lectLoc<120)//30 locations for fingerprints ID
  {
    //student fingerprint id
     EEPROM.write(studLoc,id);
  }
  //id
 */
  break;//to keep only one value at once
}

 //---------------------------------------------------------------------------------------


   
    //--------------------store students  in the SD Card file-----------------------
   myfiles2=SD.open("NOFile.txt",FILE_WRITE);
   if(myfiles2)//if file well opened
   {
    Serial.println(F("studIdFile well opened"));
    myfiles2.println(stud.studID[studCounter]);
   }
    myfiles2.close();
   //------------------------------------------------------------------------------------- 

   readingStudNo();//reading values in the file 
     
     studCounter++;

    //UPDATE studCounter VARIABLE IN THE EEPROM ADDRESS
    EEPROM.update(251,studCounter);
    userCount=studCounter;
    
    id++;// increase the number of overall registered users(lecturer,student,retaker)
    
    
  }

  //if enrolling a retaker...
  if(lectbit==0 && studbit==0 && retakerbit==1)
  {
    ret.studID[retakerCounter]=retakerCounter;
    ret.prints[retakerCounter]=id;//save the finger//printId of a student
    



 //---------------------------WRITING THE RETAKERS' DETAILS TO EEPROM------------------

  address=retLoc;
  for (retLoc;retLoc<219;retLoc++)//100 locations for 30  students retakers(120 to 219)
{
  //UPDATE retLoc VARIABLE IN THE EEPROM LOCATION
    EEPROM.update(228,retLoc);
  //writing the student's details to EEPROM
                    //registration number
     
     // if(retLoc>=90 && retLoc<120)//30 locations for registration numbers
      for(retLoc1=120;retLoc1<150;retLoc1++)
  {
    //read the previously written value in the EEPROM
    //UPDATE retLoc1 VARIABLE IN THE EEPROM LOCATION
    EEPROM.update(229,retLoc1);
    //retaker registration number
    EEPROM.put(retLoc1,ret.regNo[retakerCounter]);//for regNo[studCounter] > 255
    break;
  }

  //student id
   //if(retLoc>=120 && retLoc<150)//30 locations for retaker counter
   for(retLoc2=150;retLoc2<180;retLoc2++)
  {
    //UPDATE retLoc2 VARIABLE IN THE EEPROM LOCATION
    EEPROM.update(230,retLoc2);
    
    //student numbers
    EEPROM.write(retLoc2,ret.studID[retakerCounter]);//for studID[studCounter]
    break;
  }

   //if(retLoc>=150 && retLoc<160)//10 locations for the retaker's lessons ID
   for(retLoc3=180;retLoc3<189;retLoc3++)
  {
    //UPDATE retLoc3 VARIABLE IN THE EEPROM LOCATION
    EEPROM.update(231,retLoc3);
    //lesson ID
    EEPROM.put(retLoc3,ret.lessonID[retakerCounter]);//for lessonCode[lectCounter] > 255
    break;
  }

  //writing the retakers' fingerprint templates to EEPROM
  for(retLoc4=190;retLoc4<220;retLoc4++)
  {
    //UPDATE retLoc3 VARIABLE IN THE EEPROM LOCATION
    EEPROM.update(232,retLoc4);
    //lesson ID
    EEPROM.put(retLoc4,ret.prints[retakerCounter]);//for lessonCode[lectCounter] > 255
    break;
  }

 /*
  //student fingerprint id
   if(studLoc>=90 && lectLoc<120)//30 locations for fingerprints ID
  {
    //student fingerprint id
     EEPROM.write(studLoc,id);
  }
  //id
 */
  break;//to keep only one value at once
}

 //---------------------------------------------------------------------------------------

retakerCounter++;

    //UPDATE retakerCounter VARIABLE IN THE EEPROM ADDRESS
    EEPROM.update(252,retakerCounter);
    userCount=retakerCounter;
    
    id++;// increase the number of overall registered users(lecturer,student,retaker)
     
  }

  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(F("No:"));
/*
  if(userCount == 29)//if reached the max users
  {
    lcd.clear();
    lcd.print(F("users full"));

    //return an error
    return -1;
  }
  */
  //id=userCount;//each id corresponds to the user

  //userCount++;//increment for the next to register have next ID
  
  lcd.print(userCount);//print the user's ID
  lcd.setCursor(0,1);

  delay(2000);
    homeScreen();
  
}
//---------------------------------------------------------------------------------------

//---------------------NUMBER OF ALL USERS---UNUSED-----------------------------------
void printId(uint8_t id)//output the user's number
{
        lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(F("No:"));
  //writing the value kept in the eeprom
  lcd.print(id);
}

void retakerReg()
{
  lcd.clear();
    lcd.setCursor(0,0);
    
    lcd.print(F("lesson code:"));
    //lcd.setCursor(0,1);
    ret.lessonID[retakerLessons]=codeInput(1,4);//input codes not more than 10 digits on the second row
    
   
    //if enter is pressed
     lcd.setCursor(0,2);
    lcd.print(F("Reg. No:"));
   // lcd.setCursor(0,3);
    ret.regNo[retakerCounter]=codeInput(3,5);//input codes not more than 10 digits on the second row

    //FINGERPRINT ENROLL
    getFingerprintEnroll(0,0,1);//(uint8_t lectbit,uint8_t studbit,uint8_t retakerbit)
    
}

void readingStudNo()
{
  //--------------------store students  in the SD Card file-----------------------
   myfiles2=SD.open("NOFile.txt");
   Serial.println(F("studIdFile data"));
   while(myfiles2.available())//if file are available
   {
    
    Serial.write(myfiles2.read());
   }
    myfiles2.close();
   //------------------------------------------------------------------------------------- 
   
}

//-------------------------------- WRITING FILE TO EXCEL-------------------------
void writingToExcel()
{ 
  //CLEARING EXCEL DATA LEFT FROM THE PREV SHEET AND START @ THE 1st LINE
  //Serial.println("CLEARDATA");
  Serial.println("CLEARSHEET");
  //CREATE FIRST ROW
  Serial.println("LABEL,NO,Reg.No");
  
  
  //-------------------------------OPENING A FILE TO READ FROM------------------
  myfiles2=SD.open("NOFile.txt");
  myfiles1=SD.open("RegFile.txt");
  
   Serial.println(F("studIdFile data"));
   while((myfiles1.available()) && (myfiles2.available()))//if file are available
   {
    //ENABLING  WRITING DATA TO FIRST COLUMN
    Serial.print("DATA,");
    Serial.write(myfiles2.read());
    Serial.print(",");
    Serial.write(myfiles1.read()); 
    
    Serial.println(); //put something for the file to be read into excel
    delay(100);
   }
    myfiles1.close();
     myfiles2.close();


    //------------------------- WRITING REGISTRAION NUMBERS---------------------
}

void deleteAllPrints()
{
   //delete all fingerprint templates registered in the sensor
   finger.emptyDatabase();  
   
   
   //delete the EEPROM data
   lcd.clear();
          lcd.setCursor(5,0);
          lcd.print("FORMATING");
   for(int z=0;z<1024;z++)//EEPROM IS OF 1K MEMORY
   {
    EEPROM.write(z,0);//clear all the EEPROM locations by writing zero to all positions
   }
   
          for(int lcdrows=1;lcdrows<4;lcdrows++)//rows
          {
            
            for(int lcdcols=0;lcdcols<20;lcdcols++)//columns
            {
            
             
              lcd.setCursor(lcdcols,lcdrows);
              lcd.print(".");
              delay(100);
             
            }
            
          }
  /*
   //MUST KEEP IN MIND
   userCount=0;
    lectLoc=0;//lecturer's eeprom position counter
    lectLoc1=0;
    lectLoc2=10;
   lectLoc3=20;

  studLoc=30;//students'eeprom position counter
  studLoc1=30;
  studLoc2=60;
   studLoc3=90;

   retLoc=120;//retakers'eeprom position counter
   retLoc1=120;
   retLoc2=150;
   retLoc3=180;
   retLoc4=190;
   //THEN GO HOME 
  */
  LCD.clear();
  lcd.print("     RESET");
}


