#include "babirldrvcaen.h"

/** v7XX_set_interrupt
 *  @maddr base address
 *  @level interrupt level
 *  @n     event counter for the interrupt generation
 **/


// added by cotemba 2014/07/03
void v775_conf1(unsigned int maddr, short sval){
  
  vwrite16(maddr + V7XX_BIT_SET2, &sval);

}

void v775_conf2(unsigned int maddr, short sval){
  
  vwrite16(maddr + V7XX_BIT_CLE2, &sval);

}



void v775_crate_sel(unsigned int maddr, short sval){

  vwrite16(maddr + V7XX_CRATE_SEL, &sval);

}

void v775_evt_cnt_rst(unsigned int maddr){
  short sval;

  sval = 0x1;

  vwrite16(maddr + V7XX_EVT_CNT_RST, &sval);

}

void v775_full_scale_range(unsigned int maddr, short sval){

  vwrite16(maddr + V775_FULL_SCA_RANG, &sval);

}

void v775_threshold(unsigned int maddr, unsigned int ch, short sval){

  vwrite16(maddr + V7XX_THRESH + ch*2, &sval);

}


void v7XX_set_interrupt(unsigned int maddr, short level, short n){
  vwrite16(maddr + V7XX_INT_REG, &level);
  vwrite16(maddr + V7XX_EVT_TRIG_REG, &n);
}

void v7XX_intlevel(unsigned int maddr, short level){
  vwrite16(maddr + V7XX_INT_REG, &level);
}

int v7XX_segdata(unsigned int maddr){
  int wordcnt;

  wordcnt = 0;
  vread32(maddr+V7XX_OUTBUFF,(int *)(data+mp));
  mp += 2;
  segmentsize += 2;
  wordcnt++;

  if((data[mp-1] & V7XX_TYPE_MASK_S) == V7XX_HEADER_BIT_S){
    while(wordcnt < 34){
      /* vread32(maddr+V775_OUTBUFF+wordcnt*32,(int *)(data+mp)); */
      vread32(maddr+V7XX_OUTBUFF,(int *)(data+mp));
      mp += 2;
      segmentsize += 2;
      wordcnt++;
      if((data[mp-1] & (V7XX_TYPE_MASK_S)) != V7XX_DATA_BIT_S){
        break;
      }
    }
  }

  return segmentsize;
}


int v7XX_segdata2(unsigned int maddr){
  int wordcnt;
  int tdata;

  wordcnt = 0;
  vread32(maddr+V7XX_OUTBUFF,&tdata);

  if((tdata & V7XX_TYPE_MASK) != V7XX_HEADER_BIT){
    return 0;
  }
  data[mp++] = (tdata)&0xffff;
  data[mp++] = (tdata >> 16)&0xffff;
  segmentsize += 2;
  wordcnt++;

  while(wordcnt < 34){
    vread32(maddr+V7XX_OUTBUFF,(int *)(data+mp));
    mp += 2;
    segmentsize += 2;
    wordcnt++;
    if((data[mp-1] & (V7XX_TYPE_MASK_S)) != V7XX_DATA_BIT_S){
      break;
    }
  }

  return segmentsize;
}

int v7XX_dmasegdata(unsigned int maddr, int rsize){
  /* rsize : nanko data wo yomuka (int word wo nanko ka) */
  int wordcnt, csize;
  int staadr, stoadr, staflag, vsize;
  volatile int dmadelay, dmaflag, loop;

  wordcnt = 0;
  dmaflag = 0;

  csize = rsize * 4;  /* long word -> char size */
  vme_dma_vread32_start(maddr, csize);
  delay_us();  /* delay about 1us */
  dmadelay = 0;
  for(dmadelay=0;dmadelay<1000;dmadelay++){
    if(vme_dma_vread32_store((char *)dmadata,csize)){
      dmadelay = 5555;
      dmaflag = 1;
    }else{
      delay_us();
    }
  }

  staadr = 0; stoadr = 0;
  staflag = 0; vsize = 0;

  if(dmaflag){
    for(loop=0;loop<rsize;loop++){
      if((dmadata[loop] & V7XX_TYPE_MASK) == V7XX_HEADER_BIT){
        staadr = loop;
        staflag = 1;
        //}else if(staflag &&
        //     (dmadata[loop] & V7XX_TYPE_MASK) == V7XX_DATA_BIT){
      }else if(staflag &&
               (((dmadata[loop] & V7XX_TYPE_MASK) == V7XX_EOB_BIT) ||
                ((dmadata[loop] & V7XX_TYPE_MASK) == V7XX_ERROR_BIT))){
        stoadr = loop;
        break;
      }else if(!staflag && loop > 1){
        stoadr = 0;
        break;
      }
    }
    if(loop >= rsize){
      stoadr = rsize - 1;
    }
    vsize = stoadr - staadr + 1;

    memcpy((char *)(data+mp),(char *)(dmadata+staadr),vsize*4);

    wordcnt += vsize;
    segmentsize += vsize*2;
    mp += vsize*2;
  }else{
    data[mp++] = dmadelay & 0xffff;
    data[mp++] = 0x0600;
    wordcnt++;
    segmentsize += 2;
  }

  return segmentsize;
}

#ifdef V2718
int v7XX_fxdmasegdata(unsigned int maddr, int rsize){
  /* rsize : int word to be read */
  int csize, ret, vsize;

  csize = rsize * 4;  /* long word -> char size */
  ret = dma_vread32(maddr, (char *)(data+mp), csize);

  vsize = 0;
  if(ret >= 0){
    vsize = ret/2;
  }

  segmentsize += vsize;
  mp += vsize;

  return segmentsize;
}
#endif

void v7XX_clear(unsigned int maddr){
  short sval;

  sval = 0x04;

  vwrite16(maddr + V7XX_BIT_SET2, &sval);
  vwrite16(maddr + V7XX_BIT_CLE2, &sval);

}

#ifdef UNIV
int v7XX_multi_map_segdata(unsigned int moff, int n){
  int wordcnt;

  wordcnt = 0;
  univ_map_vread32(moff+V7XX_OUTBUFF,(int *)(data+mp), n);
  mp += 2;
  segmentsize += 2;
  wordcnt++;

  if((data[mp-1] & V7XX_TYPE_MASK_S) == V7XX_HEADER_BIT_S){
    while(wordcnt < 34){
      univ_map_vread32(moff+V7XX_OUTBUFF,(int *)(data+mp), n);
      mp += 2;
      segmentsize += 2;
      wordcnt++;
      if((data[mp-1] & (V7XX_TYPE_MASK_S)) != V7XX_DATA_BIT_S){
        break;
      }
    }
  }

  return segmentsize;
}

int v7XX_map_segdata(int n){
  return v7XX_multi_map_segdata(0, n);
}


void v7XX_multi_map_clear(unsigned int moff, int n){
  short sval;

  sval = 0x04;

  univ_map_write16(moff + V7XX_BIT_SET2, &sval, n);
  univ_map_write16(moff + V7XX_BIT_CLE2, &sval, n);
}

void v7XX_map_clear(int n){
  v7XX_multi_map_clear(0, n);
}


int v1X90_multi_map_dma_segdata(int dman, int n){
  int dmacnt;

  univ_map_read32(V1X90_EVT_FIFO, (int *)(&dmacnt), n);
  dmacnt = (dmacnt & 0x0000ffff) * 4;
  return univ_dma_segdata(dmacnt, dman);
}

int v1X90_map_dma_segdata(int n){
  /* dman = n */
  return v1X90_multi_map_dma_segdata(n, n);
}

void v1X90_multi_map_clear(unsigned int moff, int n){
  short sval = V1X90_SOFT_CLEAR;

  univ_map_write16(moff+V1290_SOFT_CLEAR, &sval, n); 
}

void v1X90_map_clear(int n){
  v1X90_multi_map_clear(0, n);
}

void v1X90_multi_map_intlevel(short level, unsigned int moff, int n){
  univ_map_write16(moff+V1X90_INT_LEVEL, &level, n);
}

void v1X90_map_intlevel(short level, int n){
  v1X90_multi_map_intlevel(level, 0, n);
}

#endif

/* added by cotemba on 15/11/11 for GAGG @CYRIC */
void v1X90_cnt_reg(unsigned int maddr, short sval){

  vwrite16(maddr + V1X90_CNT_REG, &sval);
}
void v1X90_int_level(unsigned int maddr, short sval){
  vwrite16(maddr + V1X90_INT_LEVEL, &sval);
}

void v1X90_evt_reset(unsigned int maddr){
  short sval;
  sval=0x1;
  vwrite16(maddr + V1X90_EVT_RESET, &sval);
}

void v1X90_almost_full(unsigned int maddr, short sval){
  vwrite16(maddr + V1X90_ALMOST_FULL, &sval);
}

void v1X90_soft_clear(unsigned int maddr, short sval){
  vwrite16(maddr + V1X90_SOFT_CLEAR, &sval);
}

/* programming of V1290 using OPCODE */
void v1290_trg_match(unsigned int maddr){
  short sval;


  short result;
  sval=V1290OP_TRG_MATCH;
  result=0;
// while(result!=1){
//   vread16(maddr + 0x1030, (short *)&result);
//   result=result & 0x1;
//   for(i=0;i<100;i++){
//     delay_us();
//   }
// }

  vwrite16(maddr + V1X90_MICRO_REG, &sval);

// result=0;
// while(result!=1){
//   vread16(maddr + 0x1030, (short *)&result);
//   result=result & 0x1;
//   for(i=0;i<100;i++){
//     delay_us();
//   }
// }

}

void v1290_set_win_wid(unsigned int maddr, short sval){
  short comm;
  comm=V1290OP_SET_WIN_WID;
  vwrite16(maddr + V1X90_MICRO_REG, &comm);
  vwrite16(maddr + V1X90_MICRO_REG, &sval);
}

void v1290_set_win_ofs(unsigned int maddr, short sval){
  short comm;
  comm=V1290OP_SET_WIN_OFS;
  vwrite16(maddr + V1X90_MICRO_REG, &comm);
  vwrite16(maddr + V1X90_MICRO_REG, &sval);
}

void v1290_set_sw_mrg(unsigned int maddr, short sval){
  short comm;
  comm=V1290OP_SET_SW_MRG;
  vwrite16(maddr + V1X90_MICRO_REG, &comm);
  vwrite16(maddr + V1X90_MICRO_REG, &sval);
}

void v1290_set_rej_mrg(unsigned int maddr, short sval){
  short comm;
  comm=V1290OP_SET_REJ_MRG;
  vwrite16(maddr + V1X90_MICRO_REG, &comm);
  vwrite16(maddr + V1X90_MICRO_REG, &sval);
}

void v1290_en_sub_trg(unsigned int maddr){
  short comm;
  comm=V1290OP_EN_SUB_TRG;
  vwrite16(maddr + V1X90_MICRO_REG, &comm);
}

void v1290_dis_sub_trg(unsigned int maddr){
  short comm;
  comm=V1290OP_DIS_SUB_TRG;
  vwrite16(maddr + V1X90_MICRO_REG, &comm);
}

void v1290_set_detect(unsigned int maddr, short sval){
  short comm;
  comm=V1290OP_SET_DETECT;
  vwrite16(maddr + V1X90_MICRO_REG, &comm);
  vwrite16(maddr + V1X90_MICRO_REG, &sval);
}

void v1290_set_tr_lead_LSD(unsigned int maddr, short sval){
  short comm;
  comm=V1290OP_SET_TR_LEAD_LSD;
  vwrite16(maddr + V1X90_MICRO_REG, &comm);
  vwrite16(maddr + V1X90_MICRO_REG, &sval);
}

void v1290_read_res(unsigned int maddr, short sval){
  short comm;
  comm=V1290OP_READ_RES;
  vwrite16(maddr + V1X90_MICRO_REG, &comm);
  vwrite16(maddr + V1X90_MICRO_REG, &sval);
}

void v1290_en_head_trail(unsigned int maddr){
  int i;
  short comm;
  short result;

  result=0;
  while(result!=1){
    vread16(maddr + 0x1030, (short *)&result);
    result=result & 0x1;
    for(i=0;i<100;i++){
      delay_us();
    }
  }

  comm=V1290OP_EN_HEAD_TRAIL;
  vwrite16(maddr + V1X90_MICRO_REG, &comm);

  result=0;
  while(result!=1){
    vread16(maddr + 0x1030, (short *)&result);
    result=result & 0x1;
    for(i=0;i<100;i++){
      delay_us();
    }
  }

}

void v1290_en_all_ch(unsigned int maddr){
  short comm;
  comm=V1290OP_EN_ALL_CH;
  vwrite16(maddr + V1X90_MICRO_REG, &comm);
}


int v1290_segdata(unsigned int maddr){
  int wordcnt;
  //  short evtnum;
  //  int wcnt;
  short nev;

  vread16(maddr + 0x1020, (short *)&nev);
  if(nev>1){
    printk("ERROR: V1290 multi event!, nev=%d \n", nev);
  }

  wordcnt=0;
  vread32(maddr + V1X90_OUT_BUFF, (int *)(data+mp));
  mp+=2;
  segmentsize+=2;
  wordcnt++;

  if((data[mp-1]&0xf800) == 0x4000){
    
    while((data[mp-1]&0xf800)!=0x8000){
      vread32(maddr + V1X90_OUT_BUFF, (int *)(data+mp));
      mp+=2;
      segmentsize+=2;
      wordcnt++;

      if(wordcnt>10000){
	printk("V1290 too many data!\n");
	break;
      }
    }
  }
  return segmentsize;
}
