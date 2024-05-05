#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcdUp(0x27, 40, 2);
//LiquidCrystal_I2C lcdDown(0x26, 40, 2);

#define IGNITION      52
#define BUZZER        53
#define DELAY         2000
#define BUZZER_DELAY  100   // Delay of the ignition 10 beeps

/* Global variables */
bool pressed [33];
int index;
int pressedIndicies[33];
int pressedCount;
int oldCount;

/* Messages to be displayed on lcdUp */
char countMessage[21] = "Open Doors Count=   "; // Length = 20
char ignition[21] = "    IGNITION ON     ";
String messages[33] = 
{
  "Entry Door",
  "Motorhome Slide",
  "Skylight",
  "Skylight Opaque Screen not Retracted",
  "Skylight Insect Screen is not Retracted",
  "Right Kitchen Overhead Cupboard Door",
  "Left Kitchen Overhead Cupboard Door",
  "Top Kitchen Drawer",
  "Middle Kitchen Drawer",
  "Bottom Kitchen Drawer",
  "Under-Sink Cupboard Door",
  "Wine Slide Cabinet Door",
  "Utility Under Bench Flap-Door",
  "Right Utility Under-Bench Drawer",
  "Left Utility Under-Bench Drawer",
  "IT Cupboard Door",
  "Right Floor-Level Cupboard Door",
  "Right-Centre Floor-Level Cupboard Door",
  "Left-Centre Floor-Level Cupboard Door",
  "Left Floor-Level Cupboard Door",
  "Right Floor-Level Draw",
  "Middle Floor-Level Draw",
  "Right Floor-Level Draw",
  "Bathroom Siding Door",
  "Right Under-Vanity Door (HWS)",
  "Left Bathroom Under-Vanity Door",
  "Right Over-Head Locker Door",
  "Centre Over-Head Locker Door",
  "Left Over-Head Locker Door",
  "Bathrooms Window",
  "Shore Power is still connected",
  "Driver-Side Locker",
  "Passenger-Side Locker"
};

/* Functions prototype */
void checkIgnition(void);
void checkDoors(void);
void newDoorBuzzer(void);
void displayPressedCount(void);
void displayBacklight(void);
void displayOpenedDoors(void);

void setup() 
{
  /* Initialize the lcdUp */
  lcdUp.init();
  lcdUp.clear();
  lcdUp.backlight();
  //lcdDown.init();
  //lcdDown.clear();
  //lcdDown.backlight();

  /* Initialize input pins */
  for(int i = 22; i < 52; i++)
    pinMode(i, INPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);

  /* Initialize the buzzer */
  pinMode(BUZZER, OUTPUT);

  /* Initialize the Ignition */
  pinMode(IGNITION, INPUT);
}

void loop() 
{
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

void checkIgnition(void)
{
  int beeps = 0;
  while (!digitalRead(IGNITION))
  {
    checkDoors();
    displayBacklight();
    lcdUp.setCursor(0, 0);
    lcdUp.print(ignition);
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
  lcdUp.setCursor(0, 0);
  lcdUp.print(countMessage);
}

void displayBacklight(void)
{
  if(pressedCount > 0)
    lcdUp.backlight();
  else
    lcdUp.noBacklight();
}

void displayOpenedDoors(void)
{
  index = 0;
  if(pressedCount == 1)
  {
    lcdUp.setCursor(0, 1);
    lcdUp.print(messages[pressedIndicies[index++]]);
    delay(DELAY);
    lcdUp.clear();
  }
  else if(pressedCount > 1)
  {
    lcdUp.setCursor(0, 1);
    lcdUp.print(messages[pressedIndicies[index++]]);
    delay(DELAY);
    lcdUp.clear();

    pressedCount--;
    while (pressedCount > 0)
    {
      if(pressedCount){
        lcdUp.setCursor(0, 0);
        lcdUp.print(messages[pressedIndicies[index++]]);
        pressedCount--;
      }
      if(pressedCount){
        lcdUp.setCursor(0, 1);
        lcdUp.print(messages[pressedIndicies[index++]]);
        pressedCount--;
      }
      delay(DELAY);
      lcdUp.clear();
    }
  }
}

// void displayOpenedDoors(void)
// {
//   index = 0;
//   if(pressedCount == 1)
//   {
//     lcdUp.setCursor(0, 1);
//     lcdUp.print(messages[pressedIndicies[index++]]);
//     delay(DELAY);
//     lcdUp.clear();
//     lcdDown.clear();
//   }
//   else if(pressedCount == 2)
//   {
//     lcdUp.setCursor(0, 1);
//     lcdUp.print(messages[pressedIndicies[index++]]);
//     lcdDown.setCursor(0, 0);
//     lcdDown.print(messages[pressedIndicies[index++]]);
//     delay(DELAY);
//     lcdUp.clear();
//     lcdDown.clear();
//   }
//   else if(pressedCount >= 3)
//   {
//     lcdUp.setCursor(0, 1);
//     lcdUp.print(messages[pressedIndicies[index++]]);
//     lcdDown.setCursor(0, 0);
//     lcdDown.print(messages[pressedIndicies[index++]]);
//     lcdDown.setCursor(0, 1);
//     lcdDown.print(messages[pressedIndicies[index++]]);
//     delay(DELAY);
//     lcdUp.clear();
//     lcdDown.clear();

//     pressedCount -= 3;
//     while (pressedCount > 0)
//     {
//       if(pressedCount){
//         lcdUp.setCursor(0, 0);
//         lcdUp.print(messages[pressedIndicies[index++]]);
//         pressedCount--;
//       }
//       if(pressedCount){
//         lcdUp.setCursor(0, 1);
//         lcdUp.print(messages[pressedIndicies[index++]]);
//         pressedCount--;
//       }
//       if(pressedCount){
//         lcdDown.setCursor(0, 0);
//         lcdDown.print(messages[pressedIndicies[index++]]);
//         pressedCount--;
//       }
//       if(pressedCount){
//         lcdDown.setCursor(0, 1);
//         lcdDown.print(messages[pressedIndicies[index++]]);
//         pressedCount--;
//       }
//       delay(DELAY);
//       lcdUp.clear();
//       lcdDown.clear();
//     }
//   }
// }
