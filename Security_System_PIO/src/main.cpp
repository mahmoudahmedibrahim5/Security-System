#include <Arduino.h>
#include "LiquidCrystal_I2C.h"

LiquidCrystal_I2C lcdUp(0x27, 40, 2);
LiquidCrystal_I2C lcdDown(0x23, 40, 2);

#define IGNITION  52
#define BUZZER    53

/* Global variables */
bool pressed [30];
int index;
int pressedIndicies[30];
int pressedCount;
int oldCount;

/* Messages to be displayed on lcdUp */
char countMessage[21] = "Open Doors Count=   "; // Length = 20
char ignition[21] = "    IGNITION ON     ";
String messages[30] = 
{
  "Entry Door",
  "Motorhome Slide",
  "Skylight",
  "Skylight Opaque Cover Screen isn't Retracted",
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
  "Right Bathroom Under-Vanity Door (Water Heater Area)",
  "Left Bathroom Under-Vanity Door",
  "Right Over-Head Locker Door",
  "Centre Over-Head Locker Door",
  "Left Over-Head Locker Door",
  "Bathrooms Window"
};

/* Functions prototype */
void checkIgnition(void);
void checkDoors(void);
void displayPressedCount(void);
void displayOpenedDoors(void);

void setup() 
{
  /* Initialize the lcdUp */
  lcdUp.init();
  lcdUp.clear();
  lcdDown.init();
  lcdDown.clear();

  /* Initialize input pins */
  for(int i = 22; i < 52; i++)
    pinMode(i, INPUT);

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

  /* Edit the display pressed Count */
  displayPressedCount();

  /* Edit the messages of the doors */
  displayOpenedDoors();
}

void checkIgnition(void)
{
  while (!digitalRead(IGNITION))
  {
    lcdUp.setCursor(0, 0);
    lcdUp.print(ignition);
    digitalWrite(BUZZER, HIGH);
    delay(100);
    digitalWrite(BUZZER, LOW);
    delay(100);
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

void displayOpenedDoors(void)
{
  index = 0;
  if(pressedCount == 1)
  {
    lcdUp.setCursor(0, 1);
    lcdUp.print(messages[pressedIndicies[index++]]);
    delay(1000);
    lcdUp.clear();
    lcdDown.clear();
  }
  else if(pressedCount == 2)
  {
    lcdUp.setCursor(0, 1);
    lcdUp.print(messages[pressedIndicies[index++]]);
    lcdDown.setCursor(0, 0);
    lcdDown.print(messages[pressedIndicies[index++]]);
    delay(1000);
    lcdUp.clear();
    lcdDown.clear();
  }
  else if(pressedCount >= 3)
  {
    lcdUp.setCursor(0, 1);
    lcdUp.print(messages[pressedIndicies[index++]]);
    lcdDown.setCursor(0, 0);
    lcdDown.print(messages[pressedIndicies[index++]]);
    lcdDown.setCursor(0, 1);
    lcdDown.print(messages[pressedIndicies[index++]]);
    delay(1000);
    lcdUp.clear();
    lcdDown.clear();

    pressedCount -= 3;
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
      if(pressedCount){
        lcdDown.setCursor(0, 0);
        lcdDown.print(messages[pressedIndicies[index++]]);
        pressedCount--;
      }
      if(pressedCount){
        lcdDown.setCursor(0, 1);
        lcdDown.print(messages[pressedIndicies[index++]]);
        pressedCount--;
      }
      delay(1000);
      lcdUp.clear();
      lcdDown.clear();
    }
  }
}
