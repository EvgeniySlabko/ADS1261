//--------------------- Задержка в 20 нс
void Delay20ns(void)
{
    __asm__("nop");
}
//--------------------- Задержка в 50 нс
void Delay50ns(void)
{
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
}
//--------------------- Задержка в Циклах процессора (Count >= 30 и кратен 10)
void DelayInCycles(int Count)
{
  int i, Temp;
  
  Temp = Count/10 - 2;
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
  for (i=0; i<Temp; i++) {  
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
  }
//    __asm__("nop");
}
//--------------------- Задержка в милисекундах
void DelayInMillisecond(int Count) 
{
  int i, Temp;

  Temp = Count - 1;
  for (i=0; i<Temp; i++) {  
    DelayInCycles(200000);
  }
  DelayInCycles(199840);
}
//------------------------------------------------------------------------------
