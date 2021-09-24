//--------------------- �������� � 20 ��
void Delay20ns(void)
{
    __asm__("nop");
}
//--------------------- �������� � 50 ��
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
//--------------------- �������� � ������ ���������� (Count >= 30 � ������ 10)
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
//--------------------- �������� � ������������
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
