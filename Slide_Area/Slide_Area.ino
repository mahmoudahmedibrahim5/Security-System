/* Global variables */
bool pressed [11];
bool sleep;

String receivedMessage;
String transmittedMessage;

/* Functions prototype */
void checkInputs(void);
void receiveData(void);
void sendData(void);

void setup() 
{
  Serial.begin(9600);
  sleep = true;
  for(int i = 2; i < 13; i++)
    pinMode(i, INPUT);
}

void loop() 
{
  receiveData();
  if(!sleep)
  {
    checkInputs();
    sendData();
  }
  delay(500);
}

void checkInputs(void)
{
  for(int i = 2; i < 13; i++)
    pressed[i - 2] = digitalRead(i);
}

void receiveData(void)
{
  while (!Serial.available());
  receivedMessage = Serial.readString();
  //Serial.print("Received message is: ");
  //Serial.println(receivedMessage);
  if(receivedMessage[1] == 'F')
    sleep = true;
  else if(receivedMessage[1] == 'N')
    sleep = false;
  else
  {
    Serial.println("Error");
  }
}

void sendData(void)
{
  char msg[11];
  for(int i = 0; i < 11; i++)
  {
    if(pressed[i])
      msg[i] = '1';
    else
      msg[i] = '0';
  }
  Serial.println(msg);
}
