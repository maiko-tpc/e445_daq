void clear(void){

  int i;

#ifdef USE_MADC32
  madc32_readout_reset(MADC32ADR,1);
#endif

  for(i=0; i<100; i++){
    delay_us();
  }

  v7XX_set_interrupt(V775IRQADR, 0x3, 0x1);

  rpv130_clear(RPV130ADR); // Disable interrupt

  if(mpflag){
    rpv130_output(RPV130ADR,OPBUSYCL);
  }
}
