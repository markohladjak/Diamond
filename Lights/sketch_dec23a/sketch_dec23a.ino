#include <mcp_can.h>
#include <SPI.h>

MCP_CAN CAN0(10);     // Set CS to pin 10

#define LIGHT_OFF   0x0
#define LIGHT_SIDE  0x1
#define LIGHT_LOW   0x2
#define LIGHT_HIGH  0x4
#define LIGHT_DRL   0x8

void setup()
{
  for (int i = 14; i <= 21; ++i) // A0 to A8 set INPUT_PULLUP (A0 = 14, A1 = 15,... A7 = 21)
    pinMode(i, INPUT_PULLUP);

  Serial.begin(115200);

  if(CAN0.begin(MCP_ANY, CAN_250KBPS, MCP_8MHZ) == CAN_OK) Serial.println("MCP2515 Initialized Successfully!");
  else Serial.println("Error Initializing MCP2515...");

  CAN0.setMode(MCP_NORMAL);   // Change to normal mode to allow messages to be transmitted
}

byte frame[8] = {0x12, 0x00, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00};

bool active = false;

void SetLights(byte data);
void PrintState(byte state);

void loop()
{
  bool side = analogRead(A1) < 100;
  bool low = analogRead(A2) < 100;
  bool high = analogRead(A3) < 100;
  bool drl = analogRead(A4) < 100;

  byte state = side * LIGHT_SIDE + low * LIGHT_LOW + high * LIGHT_HIGH + drl * LIGHT_DRL;
  
  if( state )
    SetLights( state );
  else if (active) 
    SetLights( LIGHT_OFF );
 
  delay(100);
}

void SetLights(byte state)
{
    frame[3] = state << 4;
    CAN0.sendMsgBuf(0x750, 0, 8, frame);

    active = (bool)state;
    
    PrintState(state);
}

void PrintState(byte state)
{
    if (state & LIGHT_SIDE) Serial.print("габарит ");
    if (state & LIGHT_LOW) Serial.print("ближнє ");
    if (state & LIGHT_HIGH) Serial.print("дальнє ");
    if (state == LIGHT_OFF) Serial.print("вимкнено ");
    
    Serial.println();
}
