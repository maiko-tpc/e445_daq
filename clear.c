void clear(void){
  int i;

  //  printk("clear routine \n");

#ifdef USE_MADC32
  //  madc32_clear(MADC32ADR);
  madc32_readout_reset(MADC32ADR,1);
  //  madc32_readout_reset(MADC32ADR,0);
  //  madc32_start_acq(MADC32ADR);
#endif
  //  v7XX_clear(V775IRQADR);
  //  v7XX_clear(V775IRQADR);
  v7XX_set_interrupt(V775IRQADR, 0x3, 0x1);
  for(i=0; i<100; i++){
    delay_us();
  }
  rpv130_clear(RPV130ADR); // Disable interrupt
  //  rpv130_enable(RPV130ADR);
  //rpv130_enable(RPV130ADR);
  if(mpflag){
    rpv130_output(RPV130ADR,OPBUSYCL);
  }
  //rpv130_enable(RPV130ADR);
  //rpv130_enable(RPV130ADR);

  // Muticlient case, you must send the end-of-busy to trigger circuits
  // this clearing function must be in here not in evt.c
  // rpv130_pulse(RPVADDR, 1);
}
