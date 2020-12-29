#define ON true
#define OFF false

#define BLINK_DELAY 200
#define READY_DELAY 200

class CRele
{
  struct {
    bool R1 = false
       , R2 = false
       , R3 = false;
  } state;

  unsigned long blink_time = 0;
  unsigned long ready_time = 0;
  bool blink_active = false;
  bool ready_active = false;
  
  int reles[3] = {3, 4, 5}; // Rele Pins

  void read_state() {
    state.R1 = digitalRead(reles[0]);
    state.R2 = digitalRead(reles[1]);
    state.R3 = digitalRead(reles[2]);
  }

  void rele_onoff(int rele_pin, bool on) {
    digitalWrite(rele_pin, on);
  }

  void power_off() {
    for (int i = 0; i < 3; i++)
      rele_onoff(reles[i], OFF);
  }

  void update_blink()
  {
      unsigned long t = millis();
      
      if (t - blink_time > BLINK_DELAY)
      {
        if (state.R1) {
          rele_onoff(reles[0], OFF);
          rele_onoff(reles[1], ON);
        } else {
          rele_onoff(reles[0], ON);
          rele_onoff(reles[1], OFF);
        }

        blink_time = t;
      }
  }

  void update_ready()
  {
      unsigned long t = millis();
      
      if (t - ready_time > READY_DELAY)
      {
        rele_onoff(reles[2], !state.R3);

        ready_time = t;
      }
  }
  
  public:
  void setup()
  {
    for (int i = 0; i < 3; i++) {
      pinMode(reles[i], OUTPUT);
      rele_onoff(reles[i], OFF);
    }
  }

  bool IsOn() { return state.R1 || state.R2 || state.R3; }

  void update()
  {
    read_state();
    if (blink_active) update_blink();
    if (ready_active) update_ready();
  }
  
  void Stop(){
    BlinkStop();
    ReadyStop();
    power_off();
  }

  void Set(int rele_num, bool state)
  {
    rele_onoff(reles[rele_num], state);
  }

  void BlinkStart()
  {
    blink_active = true;
  }

  void BlinkStop()
  {
    blink_active = false;
  }

  void ReadyStart()
  {
    ready_active = true;
  }

  void ReadyStop()
  {
    ready_active = false;
  }
};
