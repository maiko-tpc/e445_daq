void sca(void){

#ifdef USE_SCALER
  int i;
  init_ncscaler(SCRID);
  for(i=0;i<16;i++) {
    vread32(V560ADR+0x10+i*4,(unsigned int *)(data+mp));
    mp+=2;
    scrsize+=2;
  }
  end_ncscaler32();
#endif
}
