#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 40, 2);

#define IGNITION      52
#define BUZZER        53
#define DELAY         2000
#define BUZZER_DELAY  100   // Delay of the ignition 10 beeps

/* Global variables */
bool ignitionState;
bool pressed [45];
int index;
int pressedIndicies[45];
int pressedCount;
int oldCount;

/* Messages to be displayed on lcd */
char countMessage[21] = "Open Doors Count=   "; // Length = 20
char ignition[21] = "    IGNITION ON     ";
String messages[45] = 
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

/* Functions prototype */
void checkIgnition(void);
void sendIgnitionState(void);
void checkDoors(void);
void newDoorBuzzer(void);
void displayPressedCount(void);
void displayBacklight(void);
void displayOpenedDoors(void);

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

  /* Initialize the buzzer */
  pinMode(BUZZER, OUTPUT);

  /* Initialize the Ignition */
  pinMode(IGNITION, INPUT);
}

void loop() 
{
  /* Arduino UNO Inputs */
  sendIgnitionState();

  /* Check IGNITION */
  checkIgnition();

  /* Check the doors */
  checkDoors();

  /* Buzzer if new door is opened */
  newDoorBuzzer();

  /* Edit the display pressed Count */
  displayPressedCount();

  /* Update the backlight */
  displayBacklight();

  /* Edit the messages of the doors */
  displayOpenedDoors();
}

/*************************************************************************************************/
/************************************* Functions Declaration *************************************/
/*************************************************************************************************/

void sendIgnitionState(void)
{
  ignitionState = digitalRead(IGNITION);
  if(ignitionState)
  {
    Serial.println("OFF");
    for(int i = 0; i < 11; i++)
    {
      pressed[34 + i] = 0;
    }
  }
  else
  {
    Serial.println("ON");
    while (!Serial.available());
    String receivedMessage;
    receivedMessage = Serial.readString();
    for(int i = 0; i < 11; i++)
    {
      if(receivedMessage[i] == '0')
        pressed[34 + i] = 0;
      else if(receivedMessage[i] == '1')
        pressed[34 + i] = 1;
      else
        Serial.println("Error");
    }
  }
}

void checkIgnition(void)
{
  int beeps = 0;
  while (!digitalRead(IGNITION))
  {
    checkDoors();
    displayBacklight();
    lcd.setCursor(0, 0);
    lcd.print(ignition);
    if(beeps >= 10)
    {
      digitalWrite(BUZZER, HIGH);
      delay(DELAY);
      digitalWrite(BUZZER, LOW);
      delay(DELAY);
    }
    else
    {
      digitalWrite(BUZZER, HIGH);
      delay(BUZZER_DELAY);
      digitalWrite(BUZZER, LOW);
      delay(BUZZER_DELAY);
      beeps++;
    }
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

  /* Inputs connected to Arduino UNO */
  for(int i = 34; i < 45; i++)
  {
    if(pressed[i]){
      pressedCount++;
      pressedIndicies[index++] = i;
    }
  }
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
    delay(DELAY);
    lcd.clear();
  }
  else if(pressedCount > 1)
  {
    lcd.setCursor(0, 1);
    lcd.print(messages[pressedIndicies[index++]]);
    delay(DELAY);
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
      delay(DELAY);
      lcd.clear();
    }
  }
}
