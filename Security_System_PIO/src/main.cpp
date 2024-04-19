#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

#define BUZZER  53

bool pressed [30];
int index;
int pressedIndicies[30];
int pressedCount;
int oldCount;

char countMessage[21] = "Open Doors Count=   "; // Length = 20
char emptyLine[21] = "                    ";
String messages[30] = 
{
  "Entry",
  "Motorhome Slide",
  "Skylight",
  "Skylight Opaque Cover Screen is not Retracted ", // not a door
  "Skylight Insect Screen is not Retracted ", // not a door
  "Right Kitchen Overhead Cupboard",
  "Left Kitchen Overhead Cupboard",
  "Top Kitchen Drawer",
  "Middle Kitchen Drawer",
  "Bottom Kitchen Drawer",
  "Under-Sink Cupboard",
  "Wine Slide Cabinet",
  "Utility Under Bench Flap-Door",
  "Right Utility Under-Bench Drawer",
  "Left Utility Under-Bench Drawer",
  "IT Cupboard",
  "Right Floor-Level Cupboard",
  "Right-Centre Floor-Level Cupboard",
  "Left-Centre Floor-Level Cupboard",
  "Left Floor-Level Cupboard",
  "Right Floor-Level Draw",
  "Middle Floor-Level Draw",
  "Right Floor-Level Draw",
  "Bathroom Siding",
  "Right Bathroom Under-Vanity ", // not a door
  "Left Bathroom Under-Vanity",
  "Right Over-Head Locker",
  "Centre Over-Head Locker",
  "Left Over-Head Locker",
  "Bathrooms Window"
};

void setup() 
{
  /* Initialize the LCD */
  lcd.init();
  lcd.clear();

  /* Initialize input pins */
  for(int i = 22; i < 52; i++)
    pinMode(i, INPUT);

  /* Initialize the buzzer */
  pinMode(BUZZER, OUTPUT);
}

void loop() 
{
  /* Check the doors */
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

  /* Edit the display */
  if(pressedCount > 9)
    countMessage[18] = pressedCount / 10 + '0';
  else
    countMessage[18] = ' ';
  countMessage[19] = pressedCount % 10 + '0';
  lcd.setCursor(0, 0);
  lcd.print(countMessage);

  index = 0;
  while (pressedCount > 0)
  {
    for(int i = 0; i < 3; i++)
    {
      lcd.setCursor(0, i + 1);
      if(pressedCount > 0)
        lcd.print(messages[pressedIndicies[index++]]);
      pressedCount--;
    }
    delay(1000);
    lcd.clear();
  }
}

