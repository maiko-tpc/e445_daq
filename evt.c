/* evt.c for MAIKo measurement 2016/09/04  */

#include "madc32.h"
#include "babirldrvcaen.h"
#include "tmb2.h"

#define _DEBUG_EVT 1
#define _DMA_V775 0
#define _DMA_MADC32 0
#define _DMA_TMB2 0

#define LBUF_V775 1088
#define LBUF_MADC32 1026

#define FADC_DATA 0x2
#define TPC_DATA 0x3
#define FOOTER_DATA 0xffffffff

void evt(void){
  v7XX_set_interrupt(V775IRQADR, 0x0, 0x1); // disable interrupt
  rpv130_output(RPV130ADR, 0x80);

  /* MADC parameters */
  int ievt_madc;   // Event number of each event
  unsigned int time_madc; // Time stamp of each event

  /* V775 parameters */
  short wcnt_v775; // Word counts of the buffer
  int nw_v775; // Number of hit channels in each event
  int ievt_v775; // Event number of each event


  /* TMB2 parameters */
  int icn,imem;
#ifdef USE_TMB2
  int ievt_tmb2[TMB2_NMEM][TMB2_NCN]; /* Event number of each event */
  int depth[TMB2_NMEM][TMB2_NCN];
  //  size_t ccnt_tmb2,wcnt_tmb2;
#endif


#if _DEBUG_EVT > 0
  printk("\nEnter evt.c %d.\n", evtn);
#endif

 again:
  vme_read_intvector();
  rpv130_clear(RPV130ADR); // Disable interrupt

//
//
  /* Initialization *//////////////////
  wcnt_v775=0;                       //
  time_madc=0;                       //
  nw_v775=0;                         //
  ievt_v775=0;                       //
  ievt_madc=0;                       //
  for(imem=0;imem<TMB2_NMEM;imem++){ //
    for(icn=0;icn<TMB2_NCN;icn++)    //
      ievt_tmb2[imem][icn]=0;        //
  }                                  //
                                     //
///////////////////////////////////////


  // Stop Memory
  for(imem=0;imem<TMB2_NMEM;imem++){
    tmb2_stop(tmb2adr[imem]); // stop memory
    while(1){
      if(!(tmb2_readstat(tmb2adr[imem],0)&TMB2_STAT_RUN)) break;
      delay_us();
      tmb2_stop(tmb2adr[imem]); // stop memory
    }
  }


  // Read memory depth
  for(imem=0;imem<TMB2_NMEM;imem++){
    for(icn=0;icn<TMB2_NCN;icn++) {// Read depth
      depth[imem][icn]=tmb2_readcnt(tmb2adr[imem],icn);

#if _DEBUG_EVT > 0
      printk("TMB2:imem:%d  icn:%d  depth:%d\n",
	     imem,icn,depth[imem][icn]);
#endif
    }
    tmb2_switchbuf(tmb2adr[imem]); // Switch buffer
    tmb2_reset(tmb2adr[imem]); // Reset counter
  }

  for(imem=0;imem<TMB2_NMEM;imem++){
    tmb2_start(tmb2adr[imem]); // Restart Memory

    for(icn=0;icn<TMB2_NCN;icn++){
      while(1){
	if((tmb2_readstat(tmb2adr[imem],icn)&TMB2_STAT_RUN)) break;
	delay_us();
	tmb2_start(tmb2adr[imem]); // Restart Memory
      }
    }
  }

#if _DEBUG_EVT > 1
  printk("TMB2:Buffer has been switched.\n");
#endif

  rpv130_level(RPV130ADR,(OPDAQON|OPTMB2BFCH)); 

//***************** INIT EVENT *********************
  init_event();

/////////////
// Read V775
//////////////
#ifdef USE_CAEN
  /* Readout of V775 for SSD */ 
  init_segment(MKSEGID(RCNPEN,F3,SSDT,V1290));
  v1290_segdata(V1290ADR);
  end_segment();
  /* Readout of V775 for SSD to here */ 
#endif
  
  /* readout of v775 for irq */
  init_segment(MKSEGID(RCNPEN,F3,NAIT,V775));
  v7XX_segdata(V775IRQADR); //dangerous
  end_segment();
  //  v7XX_clear(V775IRQADR); // added on 16/09/06
  

/////////////
// Read MADC32
//////////////
#ifdef USE_MADC32
  init_segment(MKSEGID(RCNPEN,F3,SSDE,MADC32));
  madc32_segdata(MADC32ADR);
  end_segment();
#endif

  /* busy clear */
  if(mp<MAXBUFF){
    rpv130_output(RPV130ADR,OPBUSYCL);
    mpflag=0;
  }
  else mpflag=1;
#if _DEBUG_EVT > 0
  printk("mpflag:%d\n",mpflag);
#endif

  //////////////
  // Read TMB2
  //////////////
#if _DEBUG_EVT > 0
  printk("TMB2: Read begin.\n");
#endif
  for(imem=0;imem<TMB2_NMEM;imem++){
    for(icn=0;icn<TMB2_NCN;icn++) {
      int tmpmp,tmpct;
      /* Set seg ID of MADC (device=7, focal=19, detector=44--47, module=50) */
      init_segment(MKSEGID(RCNPEN,F3,(44+imem*2+icn),TMB2));
      tmpmp=mp;
      #if _DMA_TMB2
      tmpct=tmb2_dmasegdata(tmb2adr[imem],icn,
      			    depth[imem][icn],(int *)(data+mp));
      #else
      tmpct=tmb2_segdata(tmb2adr[imem],icn,depth[imem][icn],
			 (int *)(data+mp));
      #endif


      printk("TMB2: number of word is %d\n",tmpct);

            mp+=(depth[imem][icn])*2;
            segmentsize+=(depth[imem][icn])*2;

#if _DEBUG_EVT > 2
      printk("TMB2: Read %d counts from mem:%d cn:%d depth:%d.\n",
             tmpct,imem,icn,depth[imem][icn]);
#if _DEBUG_EVT > 3
      if(1){
	int itmp;
	for(itmp=0;itmp<depth[imem][icn];itmp++)
	  printk("TMB2: imem:%d:icn%d:%d %08x  depth:%d\n",
		 imem,icn,itmp,*((int *)(data+tmpmp+itmp*2)),depth[imem][icn]);
      }
#endif
#endif
      end_segment();
    }
  }
  
  rpv130_level(RPV130ADR,OPDAQON); 
  
  end_event();
  
  if(!mpflag){
    unsigned short irpv;
    set_amsr(0x29);
    vread16(RPV130ADR+RPV130_RSFF,&irpv);
    set_amsr(0x09);
    
    if(irpv&0x1) {
#if _DEBUG_EVT > 0
      printk("Event occurs during excuting evt.c.\n");
#endif
      printk("Event occurs during excuting evt.c.\n");
      goto again;
    }
  }
  
  //  }
  //
  //  ///////////////////
  //#if _DEBUG_EVT > 0
  //  printk("Exit from evt.c.\n");
  //#endif
}



