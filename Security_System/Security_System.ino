#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 40, 2);

#define IGNITION        52    // Ignition Pin
#define BUZZER          53    // Buzzer Pin
#define LCD_DELAY       2000  // Delay between changing the written on the LCD 2 seconds
#define BUZZER_DELAY    100   // Delay of the buzzer beep 100ms
#define SERIAL_TIMEOUT  5000  // timeout while waiting to receive from Arduino UNO (Slide Area)

/* Global variables */
bool ignitionState;
bool pressed [46];
int index;                // used for indexing in pressedIndicies array
int pressedIndicies[46];  // Indicies of pressed inputs for their messages
int pressedCount;         // Number of pressed inputs
int oldCount;             // This variable used for check if new door is opened

/* Timing variables */
int buzzerDelay;  // buzzer delay (6sec at important pins and 1 minute at rest of the pins)
unsigned long long previousBeep;    // This variable checks time between each buzzer beeps dependeing on the value of buzzerDelay
unsigned long long previousSerial;  // This variable used for timeout while waiting to receive from Arduino UNO (Slide Area)

/* Messages to be displayed on lcd */
char countMessage[21] = "Open Doors Count=   "; // Length = 20
char ignition[21] = "    IGNITION ON     ";

String messages[46] = 
{
  "*** MOTORHOME SLIDE IS OUT ",
  " ENTRY DOOR IS OPEN ",
  " BATHROOM DOOR IS UNSECURED ",
  " SKYLIGHT HATCH IS OPEN ***",
  "EXT-LOCKER IS OPEN - PASSENGER SIDE",
  "EXT-LOCKER IS OPEN - DRIVER SIDE",
  "Skylight Opaque Screen is not Retracted",
  "Skylight Insect Screen is not Retracted",
  "Kitchen Window is Open",
  "Right Kitchen O/head Cupboard Door Open",
  "Left Kitchen O/head Cupboard Door Open",
  "Top Kitchen Drawer is Hanging Open",
  "Middle Kitchen Drawer is Hanging Open",
  "Bottom Kitchen Drawer is Hanging Open",
  "Under-Sink Cupboard Door is Open",
  "Wine Slide Cabinet Door is Hanging Open",
  "Utility U/Bench Flap-Door is Open",
  "Rt Utility U/Bench Drawer Hanging Open",
  "Lt Utility U/Bench Drawer Hanging Open",
  "Starlink IT Cabinet Door is Open",
  "Right Floor-Level Cupboard Door is Open",
  "Rt-Centre Floor-Lvl Cupboard Door Open",
  "Lt-Centre Floor-Lvl Cupboard Door Open",
  "Left Floor-Level Cupboard Door is Open",
  "Right Floor-Level Drawer is Hanging Open",
  "Mid Floor-Level Drawer is Hanging Open",
  "Rt Floor-Level Drawer is Hanging Open",
  "Right Under-Vanity Door (HWS) is Open",
  "Left Bathroom Under-Vanity Door is Open",
  "Right Over-Head Locker Door is Open",
  "Centre Over-Head Locker Door is Open", 
  "Left Over-Head Locker Door is Open",   
  "Shower Ventilation Port is Open",      
  "Bathroom Window is Open",
  "** Steps Failed to Retract **",              
  "*** SHORE POWER IS STILL CONNECTED ***",
  "Left Over-bed Locker is Open",
  "Right Over-bed Locker is Open",
  "Top Bedside Wardrobe Open",
  "Bottom Bedside Wardrobe Open",
  "Bed Window is Open",
  "Left Diner Locker is Open",
  "Right Diner Locker is Open",
  "Diner Window is Open",
  "Lithium Battery Cabinet is Open",
  "Inverter Cabinet is Open"
};

/*************************************************************************************************/
/************************************** Functions prototypes *************************************/
/*************************************************************************************************/

/* This function check the ignition input
 *  if the ignition is on do nothing
 *  if the ignition is off tell the Arduino UNO (Slide Area) to sleep and wait for it to be on
 *  when it becomes on wake the Arduino UNO (Slide Area) and beep the buzzer 10 beeps 
 */
void checkIgnition(void);

/* This Function Wait to receive the data of the inputs from the Arduino UNO 
 * Waiting with timeout so that it doesn't stuck here if the UNO is broken
 * it parse the recieved string to know which inputs are pressed and which are not
 * it modifies the 11 elements from 35 to 45 in pressed array
 */
void receiveSlideAreaInputs(void);

/* This functions check the inputs and update pressed and pressedIndicies arrays
 * 30 Inputs connected from pin 22 to pin 52 
 * 5 Inputs connected to A0, A1, A2, A3, A4
 * Update pressed and pressedIndicies arrays for Arduino UNO (Slide Area) inputs
 * Update the buzzerDelay depending on if important pin is pressed or not
 * Important pins are 22, 25, 51 and pin 2 in the Arduino UNO (Slide Area) 
 * There indicies in arrays are 0, 3, 34, 35
 */
void checkDoors(void);

/* This function make a buzzer beep if new input is pressed
 * It checks the number of previous pressed inputs and the current pressed inputs
 */
void newDoorBuzzer(void);

/* This function make a continous buzzer beeps if there are pressed inputs 
 * It make beep every minute in normal, but it make beep every 6 seconds if important pin is pressed
 */
void buzzer(void);

/* This function display pressed count message on the LCD */
void displayPressedCount(void);

/* This function updates backlight depending on wether ther are pressed inputs or not */
void displayBacklight(void);

/* This function display the messages corresponding to each pressed input */
void displayOpenedDoors(void);

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

void setup() 
{
  /* Initialize The Bluetooth Module */
  Serial.begin(9600);

  /* Initialize the lcd */
  lcd.init();
  lcd.clear();
  lcd.backlight();

  /* Initialize input pins */
  for(int i = 22; i < 52; i++)
    pinMode(i, INPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);

  /* Initialize the buzzer */
  pinMode(BUZZER, OUTPUT);

  /* Initialize the Ignition */
  pinMode(IGNITION, INPUT);
}

void loop() 
{
  /* Check IGNITION */
  checkIgnition();

  /* Arduino UNO Inputs */
  receiveSlideAreaInputs();

  /* Check the doors */
  checkDoors();

  /* Buzzer if new door is opened */
  newDoorBuzzer();

  /* Continous buzzer */
  buzzer();

  /* Update the display pressed Count */
  displayPressedCount();

  /* Update the backlight */
  displayBacklight();

  /* Update the messages of the doors */
  displayOpenedDoors();
}

/*************************************************************************************************/
/************************************ Functions Implementaion ************************************/
/*************************************************************************************************/

void checkIgnition(void)
{
  if(digitalRead(IGNITION)) // If the ignition is off
  {
    /* Sleep the Arduino UNO (Slide Area) */
    Serial.println("OFF");  
    /* Turn Off LCD */
    lcd.clear();        
    lcd.noBacklight();
    
    /* Wait until it's on */
    while (digitalRead(IGNITION));  
    
    previousBeep = millis(); // Initialize this variable for buzzer beeps if there is pressed input

    /* 10 Wakeup Beeps */
    for(int i = 0; i < 10; i++)
    {
      digitalWrite(BUZZER, HIGH);
      delay(BUZZER_DELAY);
      digitalWrite(BUZZER, LOW);
      delay(BUZZER_DELAY);
    }
  }
  /* Wake the Arduino UNO (Slide Area) */
  Serial.println("ON");
}


void receiveSlideAreaInputs(void)
{
  bool slideAreaLife = true;
  previousSerial = millis();
  while (!Serial.available())
  {
    if(millis() - previousSerial > SERIAL_TIMEOUT){
      slideAreaLife = false;
      break;
    }
  }

  if(slideAreaLife)
  {
    String receivedMessage;
    receivedMessage = Serial.readString();
    for(int i = 0; i < 11; i++)
    {
      if(receivedMessage[i] == '0')
        pressed[35 + i] = 0;
      else if(receivedMessage[i] == '1')
        pressed[35 + i] = 1;
      else
        Serial.println("Error");
    }
  }
  else
  {
    for(int i = 0; i < 11; i++)
        pressed[35 + i] = 0; 
  }
}


void checkDoors(void)
{
  pressedCount = 0;
  index = 0;
  for(int i = 22; i < 52; i++)
  {
    pressed[i - 22] = digitalRead(i);
    if(pressed[i -22]){
      pressedCount++;
      pressedIndicies[index++] = i - 22;
    }
  }
  
  pressed[30] = digitalRead(A0);
  if(pressed[30]){
    pressedCount++;
    pressedIndicies[index++] = 30;
  }
  
  pressed[31] = digitalRead(A1);
  if(pressed[31]){
    pressedCount++;
    pressedIndicies[index++] = 31;
  }
  
  pressed[32] = digitalRead(A2);
  if(pressed[32]){
    pressedCount++;
    pressedIndicies[index++] = 32;
  }

  pressed[33] = digitalRead(A3);
  if(pressed[33]){
    pressedCount++;
    pressedIndicies[index++] = 33;
  }

  pressed[34] = digitalRead(A4);
  if(pressed[34]){
    pressedCount++;
    pressedIndicies[index++] = 34;
  }

  /* Inputs connected to Arduino UNO */
  for(int i = 35; i < 46; i++)
  {
    if(pressed[i]){
      pressedCount++;
      pressedIndicies[index++] = i;
    }
  }

  /* Continous Buzzer Delay */
  if(pressed[0] || pressed[3] || pressed[34] || pressed[35]) // Important pins
    buzzerDelay = 1000;
  else
    buzzerDelay = 60000;
}


void newDoorBuzzer(void)
{
  if(oldCount < pressedCount){
    digitalWrite(BUZZER, HIGH);
    delay(500);
    digitalWrite(BUZZER, LOW);
  }
  oldCount = pressedCount;
}


void buzzer()
{
  if(millis() - previousBeep > buzzerDelay){
    previousBeep = millis();
    digitalWrite(BUZZER, HIGH);
    delay(BUZZER_DELAY);
    digitalWrite(BUZZER, LOW);
    delay(BUZZER_DELAY);
  }
}


void displayPressedCount(void)
{
  if(pressedCount > 9)
    countMessage[18] = pressedCount / 10 + '0';
  else
    countMessage[18] = ' ';
  countMessage[19] = pressedCount % 10 + '0';
  lcd.setCursor(0, 0);
  lcd.print(countMessage);
}


void displayBacklight(void)
{
  if(pressedCount > 0)
    lcd.backlight();
  else
    lcd.noBacklight();
}


void displayOpenedDoors(void)
{
  index = 0;
  if(pressedCount == 1)
  {
    lcd.setCursor(0, 1);
    lcd.print(messages[pressedIndicies[index++]]);
    delay(LCD_DELAY);
    lcd.clear();
  }
  else if(pressedCount > 1)
  {
    lcd.setCursor(0, 1);
    lcd.print(messages[pressedIndicies[index++]]);
    delay(LCD_DELAY);
    lcd.clear();

    pressedCount--;
    while (pressedCount > 0)
    {
      if(pressedCount){
        lcd.setCursor(0, 0);
        lcd.print(messages[pressedIndicies[index++]]);
        pressedCount--;
      }
      if(pressedCount){
        lcd.setCursor(0, 1);
        lcd.print(messages[pressedIndicies[index++]]);
        pressedCount--;
      }
      delay(LCD_DELAY);
      lcd.clear();
    }
  }
}
