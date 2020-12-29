#include "CRele.h"
#include "CSensors.h"

CRele rele;
CSensors sensors;

void setup() {
  Serial.begin(9600);

  rele.setup();

  sensors.RegisterButton(A2, OnFree, NULL);
  sensors.RegisterButton(A3, OnSos, NULL);
  sensors.RegisterButton(A4, OnReady, NULL);
  sensors.RegisterButton(A5, OnWork, NULL);
}

void loop() {
  rele.update();  
  sensors.update();
}

void OnWork()
{
  rele.Stop();
  rele.Set(0, ON);
  Serial.println("OnWork");
}

void OnReady()
{
  rele.Stop();
  rele.ReadyStart();
  Serial.println("OnReady");
}

void OnFree()
{
  rele.Stop();
  rele.Set(2, ON);
  Serial.println("OnFree");
}

void OnSos()
{
  rele.Stop();
  rele.BlinkStart();
  Serial.println("OnSos");
}
