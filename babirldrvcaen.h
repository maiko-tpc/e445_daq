/* define */
#define v775_segdata      v7XX_segdata
#define v785_segdata      v7XX_segdata
#define v792_segdata      v7XX_segdata
#define v775_clear        v7XX_clear
#define v785_clear        v7XX_clear
#define v792_clear        v7XX_clear
#define v775_set_interrupt        v7XX_set_interrupt
#define v785_set_interrupt        v7XX_set_interrupt
#define v792_set_interrupt        v7XX_set_interrupt
#define v1190_segdata     v1X90_segdata
#define v1290_segdata     v1X90_segdata
#define v1190_clear       v1X90_clear
#define v1290_clear       v1X90_clear
#define v1190_dma_segdata v1X90_dma_segdata
#define v1290_dma_segdata v1X90_dma_segdata

/*
   V7XX, 32 channel TDC
*/

/* Address Map */
#define V7XX_OUTBUFF        0x0000   /* - 0x07ff (D32) */
#define V7XX_FREV           0x1000
#define V7XX_GEO_ADDR       0x1002
#define V7XX_MCST_ADDR      0x1004
#define V7XX_BIT_SET1       0x1006
#define V7XX_BIT_CLE1       0x1008
#define V7XX_INT_REG        0x100a
#define V7XX_INT_VEC        0x100c
#define V7XX_STS_REG1       0x100e
#define V7XX_CTRL_REG1      0x1010
#define V7XX_ADER_H         0x1012
#define V7XX_ADER_L         0x1014
#define V7XX_SS_RST         0x1016
#define V7XX_MCST_CTRL      0x101a
#define V7XX_EVT_TRIG_REG   0x1020
#define V7XX_STS_REG2       0x1022
#define V7XX_EVT_CNT_L      0x1024
#define V7XX_EVT_CNT_H      0x1026
#define V7XX_INC_EVT        0x1028
#define V7XX_INC_OFF        0x102a
#define V7XX_LOAD_TEST_REG  0x102c
#define V7XX_FCLR_WINDOW    0x102e
#define V7XX_BIT_SET2       0x1032
#define V7XX_BIT_CLE2       0x1034
#define V7XX_WMEM_TEST      0x1036
#define V7XX_MEM_TEST_H     0x1038
#define V7XX_MEM_TEST_L     0x103a
#define V7XX_CRATE_SEL      0x103c
#define V7XX_TEST_EVT       0x103e
#define V7XX_EVT_CNT_RST    0x1040
#define V775_FULL_SCA_RANG  0x1060
#define V7XX_RTEST          0x1064
#define V7XX_SW_COMM        0x1068
#define V7XX_SLIDE_CONST    0x106a
#define V7XX_AAD            0x1070
#define V7XX_BAD            0x1072
#define V7XX_THRESH         0x1080 /* - 0x10BF */

/* Bits */
#define V7XX_TYPE_MASK      0x06000000
#define V7XX_HEADER_BIT     0x02000000
#define V7XX_DATA_BIT       0x00000000
#define V7XX_EOB_BIT        0x04000000
#define V7XX_ERROR_BIT      0x06000000
#define V7XX_TYPE_MASK_S    0x0600
#define V7XX_HEADER_BIT_S   0x0200
#define V7XX_DATA_BIT_S     0x0000
#define V7XX_EOB_BIT_S      0x0400
#define V7XX_ERROR_BIT_S    0x0600



/* For V1190, V1290 */
/* Register */
#define V1X90_OUT_BUFF        0x0000
#define V1X90_INT_LEVEL       0x100a
#define V1X90_SOFT_CLEAR      0x1016
#define V1X90_EVT_RESET       0x1018
#define V1X90_ALMOST_FULL     0x1022
#define V1X90_EVT_FIFO        0x1038

/* Bit */
#define V1X90_SOFT_CLEAR_BIT  1


/* added by cotemba on 15/11/11 for GAGG @CYRIC */
#define V1X90_CNT_REG         0x1000
#define V1X90_MICRO_REG       0x102e
#define V1X90_MICRO_HAND_REG  0x1030

/* V1290 OPCODE (see $5 in the manual) */
#define V1290OP_TRG_MATCH         0x0000
#define V1290OP_READ_ACQ_MOD      0x0200

#define V1290OP_SET_WIN_WID       0x1000
#define V1290OP_SET_WIN_OFS       0x1100
#define V1290OP_SET_SW_MRG        0x1200
#define V1290OP_SET_REJ_MRG       0x1300
#define V1290OP_EN_SUB_TRG        0x1400
#define V1290OP_DIS_SUB_TRG       0x1500
			          
#define V1290OP_SET_DETECT        0x2200
#define V1290OP_READ_DETECT       0x2300
#define V1290OP_SET_TR_LEAD_LSD   0x2400
#define V1290OP_READ_RES          0x2600
#define V1290OP_EN_HEAD_TRAIL     0x3000
#define V1290OP_EN_ALL_CH         0x4200
