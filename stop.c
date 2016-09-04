void stop(void){
  short sval;

#ifdef USE_TMB2
  tmb2_stop(TMB2ADR1);
  tmb2_stop(TMB2ADR2);
#endif

#ifdef USE_MADC32
  madc32_stop_acq(MADC32ADR);
#endif

  rpv130_clear(RPV130ADR);
  rpv130_output(RPV130ADR,OPSCASTOP);
  rpv130_level(RPV130ADR,0);
  sval=1;
  //  vwrite16(V560ADR+0x52,&sval);  // VME VETO set
  printk("DAQ stop\n\n");
}
