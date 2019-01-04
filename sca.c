void sca(void){

#ifdef USE_SCALER
  int i;
  init_ncscaler(SCRID);
  for(i=0;i<32;i++) {
    vread32(V830ADR+0x1000+i*4,(unsigned int *)(data+mp));
    mp+=2;
    scrsize+=2;
  }
  end_ncscaler32();
#endif
}
