void startup(void){
  printk("entering startup()\n");

  short sval;
  int imem,icn;

  /* Startup Function */

  /* Define the interrupt level for the controller */
  vme_define_intlevel(INTLEVEL);


  /////////////////////
  // Initialize RPV130
  /////////////////////
  vme_read_intvector();
  rpv130_clear(RPV130ADR); // Disable interrupt

  /////////////////////
  // Initialize TMB2
  /////////////////////
#ifdef USE_TMB2
  for(imem=0;imem<TMB2_NMEM;imem++){
    tmb2_reset(tmb2adr[imem]);
    for(icn=0;icn<TMB2_NCN;icn++){
      tmb2_setdepth(tmb2adr[imem],icn,TMB2DEPTH);
      printk("%x:%d:imem:%d  icn:%d  depth:%x\n",tmb2adr[imem],
	     TMB2DEPTH,imem,icn,
	     tmb2_readdepth(tmb2adr[imem],icn));
    }
    tmb2_switchbuf(tmb2adr[imem]);
    delay_us();
    tmb2_clear(tmb2adr[imem],TMB2_CONT_CLALL0);
  }
  printk("TMB2 initialize done.\n");
#endif


  /////////////////////
  // Initialize MADC32
  /////////////////////
#ifdef USE_MADC32
  int ich;
  //  evcnt_madc=0;

  madc32_resol_4k(MADC32ADR); //set resolution
  madc32_input_range(MADC32ADR, 0x1); //input range (0:4V, 1:10V, 2:8V)
  //  madc32_slc_off(MADC32ADR);
  //  madc32_skip_oorange(MADC32ADR);  //skip out of range values
  madc32_use_gg(MADC32ADR, 0x1);  // use internal gate
  madc32_hold_delay(MADC32ADR, 0, 0);  // gate delay 
  madc32_hold_width(MADC32ADR, 0, 160);  // gate width (*50 ns) 
  madc32_nim_busy(MADC32ADR, 0x1); // NIM3 output 0:busy, 1:gate0
  //  madc32_multi_event(MADC32ADR, 3); //multi event mode 3

  madc32_irq_threshold(MADC32ADR, 1);
  madc32_max_transfer_data(MADC32ADR, 1);

  /* for time stamp from here */
  madc32_marking_type(MADC32ADR, 0x1);  //00->event counter, 01->time stamp
                                        //11->extended time stamp
  madc32_NIM_gat1_osc(MADC32ADR, 0x0);  //0->gate1 input, 1->clock input
  madc32_ts_sources(MADC32ADR, 0x0);    //0->VME, 1->external


  /* Set thresholds */
  for(ich=0; ich<32; ich++){
    madc32_threshold(MADC32ADR, ich, 0x1FFF); //supress all chanels
    //    madc32_threshold(MADC32ADR, ich, 0x0); //supress all chanels
  }

  madc32_threshold(MADC32ADR, 5, 0); // enable ch5
  madc32_threshold(MADC32ADR, 6, 0);
  madc32_threshold(MADC32ADR, 7, 0);
  madc32_threshold(MADC32ADR, 8, 0);
  
  printk("MADC32 initialize done.\n");
  /* to here */
#endif  

#ifdef USE_CAEN
  //////////////////////////////////////
  //// Initialize V775 for interrupter 
  //////////////////////////////////////

  /* added by cotemba on 15/06/08 for interrupt */
  v7XX_set_interrupt(V775IRQADR, 0x3, 0x1);
  /* to here */

  v775_crate_sel(V775IRQADR, 0x0000); // crate select (see sec. 4.31)
  
  v775_conf1(V775IRQADR, 0x5800);   // Bit Set 2
  v775_conf2(V775IRQADR, ~0x5800);   // Bit Set 2
  v775_full_scale_range(V775IRQADR, 0x1E);

  sval=0x20;
  vwrite16(V775IRQADR+0x1010,&sval); // Set Control Register 1

  //////////////////////////////////////
  //// Initialize V1190s
  //////////////////////////////////////

  /* MAIKO */
  v1X90_evt_reset(V1190_MAIKO_ADR);
  v1X90_int_level(V1190_MAIKO_ADR,0x7);
  v1X90_almost_full(V1190_MAIKO_ADR, 0xffff);
  v1X90_cnt_reg(V1190_MAIKO_ADR, 0x028);

  /* BDC1 */
  v1X90_evt_reset(V1190_BDC1_ADR);
  v1X90_int_level(V1190_BDC1_ADR,0x7);
  v1X90_almost_full(V1190_BDC1_ADR, 0xffff);
  v1X90_cnt_reg(V1190_BDC1_ADR, 0x028);

  /* BDC2 */
  v1X90_evt_reset(V1190_BDC2_ADR);
  v1X90_int_level(V1190_BDC2_ADR,0x7);
  v1X90_almost_full(V1190_BDC2_ADR, 0xffff);
  v1X90_cnt_reg(V1190_BDC2_ADR, 0x028);

#endif  

#ifdef USE_SCALER
  ///////////////////////
  //// Initialize V830
  ///////////////////////

  sval=1;
  vwrite16(V830ADR+0x1122,&sval);  // Scaler Clear
#endif

  /* Start V775 */
#ifdef USE_CAEN
  v7XX_clear(V775IRQADR);  // clear data, pointers, event counter and peak section
  v775_evt_cnt_rst(V775IRQADR); //event counter reset
#endif

  /* Start MADC */
#ifdef USE_MADC32
  madc32_stop_acq(MADC32ADR);
  //  madc32_fifo_reset(MADC32ADR,1); //reset FIFO
  madc32_fifo_reset(MADC32ADR,0); 
  //  madc32_readout_reset(MADC32ADR,1); //reset FIFO
  
  madc32_reset_ctr_ab(MADC32ADR); // reset event counter or timestamp
  madc32_start_acq(MADC32ADR); //start MADC32
  madc32_readout_reset(MADC32ADR,1); 

#endif

#ifdef USE_CAEN
  vread16(V775IRQADR+0x1024,&sval);   // V775 eventcounter read
  printk("V775 event counter:%d\n",sval);
#endif

  /* Start TMB2 */
#ifdef USE_TMB2
  for(imem=0;imem<TMB2_NMEM;imem++){
    int delayloop=0;
    tmb2_start(tmb2adr[imem]);
    for(delayloop=0;delayloop<1000000;delayloop++){
      if((tmb2_readstat(tmb2adr[imem],0)&TMB2_STAT_RUN)){
	printk("imem=%d, delayloop=%d\n", imem, delayloop);
	break;
      }
      delay_us();
      tmb2_start(tmb2adr[imem]);
    }
  }
#endif

  /* Start DAQ */
  //  rpv130_enable(RPV130ADR);
  rpv130_output(RPV130ADR,OPIWKRST);
  rpv130_output(RPV130ADR,OPBUSYCL|OPDAQSTART);
  rpv130_output(RPV130ADR, OPSCACLEAR);
  rpv130_output(RPV130ADR, OPSCASTART);
  printk("DAQ start.\n");

}
