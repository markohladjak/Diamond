#include <ArduinoSTL.h>

#define BUTTON_READ_DELAY 1000


class CSensors
{
  struct BtnCtx {
    int pin = -1;
    bool butt = false;
    bool butt_flag = false;
    unsigned long last_press = 0;

    void (*key_pressed)();
    void (*key_released)();
  };
  
  std::vector<BtnCtx> buttCtxs;
   
  void read_key(struct BtnCtx &ctx)
  {
    if (ctx.butt == 1 && ctx.butt_flag == 0 && millis() - ctx.last_press > BUTTON_READ_DELAY) {
      //delay(100);
      ctx.butt_flag = 1;
      ctx.last_press = millis();
      if ( ctx.key_pressed ) ctx.key_pressed();
      //delay(200);
    }
    if (ctx.butt == 0 && ctx.butt_flag == 1) {
      ctx.butt_flag = 0;
      if( ctx.key_released ) 
        ctx.key_released();
    }
  }
  public:

  void RegisterButton(int pin, void (*key_pressed)(), void (*key_released)()){
      BtnCtx ctx;

      pinMode(pin, INPUT_PULLUP);
      ctx.pin = pin;
      ctx.key_pressed = key_pressed;
      ctx.key_released = key_released;
      
      buttCtxs.push_back(ctx);
  }
  
  void setup(){
  }

  void update(){
    for (auto &ctx: buttCtxs) {
      ctx.butt = !digitalRead(ctx.pin);
      read_key(ctx);
    }    
  }
};
