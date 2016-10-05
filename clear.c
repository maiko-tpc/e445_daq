void clear(void){

  int i;

  for(i=0; i<10; i++){
    delay_us();
  }

  v7XX_set_interrupt(V775IRQADR, 0x3, 0x1);

  rpv130_clear(RPV130ADR); // Disable interrupt

  if(mpflag){
    rpv130_output(RPV130ADR,OPBUSYCL);
  }
}
