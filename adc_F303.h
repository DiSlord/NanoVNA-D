
// F303 related ADC defines


#define ADC_SMPR_SMP_247P5      6   /**< @brief 260 cycles conversion time. */
#define ADC_SMPR_SMP_24P5       3   /**< @brief 37 cycles conversion time.  */


#define rccEnableWWDG(lp) rccEnableAPB1(RCC_APB1ENR_WWDGEN, lp)
#define ADC_CHSELR_CHSEL6  ADC_CHANNEL_IN3
#define ADC_CHSELR_CHSEL7  ADC_CHANNEL_IN4
#define ADC_SMPR_SMP_239P5      7U
#define ADC_SMPR_SMP_28P5       3U  /**< @brief 41 cycles conversion time.  */
#define ADC_CFGR_RES_12BIT             (0 << 3)
/*
msg_t adcConvert(ADCDriver *adcp,
                   const ADCConversionGroup *grpp,
                   adcsample_t *samples,
                   size_t depth);
*/
#define  ADC_CR1_AWDEN                       ((uint32_t)0x00800000)  /*!< Analog watchdog enable on regular channels */
//ADC_Common_TypeDef        *adcc;
#define ADC_CHSELR_VREFINT      ADC_CHANNEL_IN18
#define ADC_CHSELR_VBAT         ADC_CHANNEL_IN17

#define ADC_ISR_ADRDY_Pos              (0U)
#define ADC_ISR_ADRDY_Msk              (0x1U << ADC_ISR_ADRDY_Pos)             /*!< 0x00000001 */
#define ADC_ISR_ADRDY                  ADC_ISR_ADRDY_Msk                       /*!< ADC ready flag */
#define ADC_IER_ADRDYIE_Pos            (0U)
#define ADC_IER_ADRDYIE_Msk            (0x1U << ADC_IER_ADRDYIE_Pos)           /*!< 0x00000001 */
#define ADC_IER_ADRDYIE                ADC_IER_ADRDYIE_Msk                     /*!< ADC ready interrupt */
#define ADC_IER_AWD1IE_Pos             (7U)
#define ADC_IER_AWD1IE_Msk             (0x1U << ADC_IER_AWD1IE_Pos)            /*!< 0x00000080 */
#define ADC_IER_AWD1IE                 ADC_IER_AWD1IE_Msk                      /*!< ADC analog watchdog 1 interrupt */
#define ADC_IER_EOCIE_Pos              (2U)
#define ADC_IER_EOCIE_Msk              (0x1U << ADC_IER_EOCIE_Pos)             /*!< 0x00000004 */
#define ADC_IER_EOCIE                  ADC_IER_EOCIE_Msk                       /*!< ADC group regular end of unitary conversion interrupt */
#define ADC_IER_EOSMPIE_Pos            (1U)
#define ADC_IER_EOSMPIE_Msk            (0x1U << ADC_IER_EOSMPIE_Pos)           /*!< 0x00000002 */
#define ADC_IER_EOSMPIE                ADC_IER_EOSMPIE_Msk                     /*!< ADC group regular end of sampling interrupt */

