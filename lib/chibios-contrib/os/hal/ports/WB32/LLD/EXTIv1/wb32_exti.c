/*
    Copyright (C) 2023 Westberry Technology Corp., Ltd

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/**
 * @file    EXTIv1/WB32_exti.c
 * @brief   EXTI helper driver code.
 *
 * @addtogroup WB32_EXTI
 * @details EXTI sharing helper driver.
 * @{
 */

#include "hal.h"

/* The following macro is only defined if some driver requiring EXTI services
   has been enabled.*/
#if defined(WB32_EXTI_REQUIRED) || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   WB32 EXTI group 1 lines initialization.
 *
 * @param[in] mask      mask of group 1 lines to be initialized
 * @param[in] mode      initialization mode
 *
 * @api
 */
void extiEnable(uint32_t mask, extimode_t mode) {
  uint32_t cmask;
  /* Enable EXTI clock.*/
  rccEnableEXTI();

  /* Mask including only configurable lines.*/
  cmask = mask & ~WB32_EXTI_IMR1_MASK;

  if ((mode & EXTI_MODE_EDGES_MASK) == 0U) {
    /* Disabling channels.*/
    EXTI->IMR  &= ~mask;
    EXTI->EMR  &= ~mask;
    EXTI->RTSR &= ~cmask;
    EXTI->FTSR &= ~cmask;
    EXTI->PR   =  cmask;
  } else {
    /* Programming edge registers.*/
    if (mode & EXTI_MODE_RISING_EDGE) {
      EXTI->RTSR |= cmask;
    }
    else {
      EXTI->RTSR &= ~cmask;
    }
    if (mode & EXTI_MODE_FALLING_EDGE) {
      EXTI->FTSR |= cmask;
    }
    else {
      EXTI->FTSR &= ~cmask;
    }

    /* Programming interrupt and event registers.*/
    if ((mode & EXTI_MODE_ACTION_MASK) == EXTI_MODE_ACTION_INTERRUPT) {
      EXTI->IMR |= mask;
      EXTI->EMR &= ~mask;
    }
    else {
      EXTI->EMR |= mask;
      EXTI->IMR &= ~mask;
    }
  }
  
  if ((!EXTI->IMR) && (!EXTI->EMR)) {
    /* Disable EXTI clock.*/
    rccDisableEXTI();
  }
}

/**
 * @brief   WB32 EXTI line initialization.
 *
 * @param[in] line      line to be initialized
 * @param[in] mode      initialization mode
 *
 * @api
 */
void extiEnableLine(extiline_t line, extimode_t mode) {
  uint32_t mask = (1U << (line & 0x1FU));

  osalDbgCheck(line < WB32_EXTI_NUM_LINES);
  osalDbgCheck((mode & ~EXTI_MODE_MASK) == 0U);

  extiEnable(mask, mode);
}

/**
 * @brief   WB32 EXTI line IRQ status clearing.
 *
 * @param[in] line      line to be initialized
 *
 * @api
 */
void extiClearLine(extiline_t line) {
  uint32_t mask = (1U << (line & 0x1FU));

  osalDbgCheck(line < WB32_EXTI_NUM_LINES);

  extiClear(mask);
}

#endif /* WB32_EXTI_REQUIRED */

OSAL_IRQ_HANDLER(WB32_EXTI0_IRQ_VECTOR) {
    uint32_t pr;

    OSAL_IRQ_PROLOGUE();

    pr = EXTI->PR & EXTI_PR_PR0;

    if ((pr & EXTI_PR_PR0) != 0) {
        EXTI->IMR &= ~(EXTI_IMR_MR0);
    }

    NVIC_DisableIRQ(EXTI0_IRQn);

    EXTI->PR = EXTI_PR_PR0;

    OSAL_IRQ_EPILOGUE();
}

OSAL_IRQ_HANDLER(WB32_EXTI1_IRQ_VECTOR) {
    uint32_t pr;

    OSAL_IRQ_PROLOGUE();

    pr = EXTI->PR & EXTI_PR_PR1;

    if ((pr & EXTI_PR_PR1) != 0) {
        EXTI->IMR &= ~(EXTI_IMR_MR1);
    }

    NVIC_DisableIRQ(EXTI1_IRQn);

    EXTI->PR = EXTI_PR_PR1;

    OSAL_IRQ_EPILOGUE();
}

OSAL_IRQ_HANDLER(WB32_EXTI2_IRQ_VECTOR) {
    uint32_t pr;

    OSAL_IRQ_PROLOGUE();

    pr = EXTI->PR & EXTI_PR_PR2;

    if ((pr & EXTI_PR_PR2) != 0) {
        EXTI->IMR &= ~(EXTI_IMR_MR2);
    }

    NVIC_DisableIRQ(EXTI2_IRQn);

    EXTI->PR = EXTI_PR_PR2;

    OSAL_IRQ_EPILOGUE();
}

OSAL_IRQ_HANDLER(WB32_EXTI3_IRQ_VECTOR) {
    uint32_t pr;

    OSAL_IRQ_PROLOGUE();

    pr = EXTI->PR & EXTI_PR_PR3;

    if ((pr & EXTI_PR_PR3) != 0) {
        EXTI->IMR &= ~(EXTI_IMR_MR3);
    }

    NVIC_DisableIRQ(EXTI3_IRQn);

    EXTI->PR = EXTI_PR_PR3;

    OSAL_IRQ_EPILOGUE();
}

OSAL_IRQ_HANDLER(WB32_EXTI4_IRQ_VECTOR) {
    uint32_t pr;

    OSAL_IRQ_PROLOGUE();

    pr = EXTI->PR & EXTI_PR_PR4;

    if ((pr & EXTI_PR_PR4) != 0) {
        EXTI->IMR &= ~(EXTI_IMR_MR4);
    }

    NVIC_DisableIRQ(EXTI4_IRQn);

    EXTI->PR = EXTI_PR_PR4;

    OSAL_IRQ_EPILOGUE();
}

OSAL_IRQ_HANDLER(WB32_EXTI9_5_IRQ_VECTOR) {
    uint32_t pr;

    OSAL_IRQ_PROLOGUE();

    pr = EXTI->PR & (EXTI_PR_PR5 | EXTI_PR_PR6 | EXTI_PR_PR7 | EXTI_PR_PR8 | EXTI_PR_PR9);

    if ((pr & EXTI_PR_PR5) != 0) {
        EXTI->IMR &= ~(EXTI_IMR_MR5);
    }

    if ((pr & EXTI_PR_PR6) != 0) {
        EXTI->IMR &= ~(EXTI_IMR_MR6);
    }

    if ((pr & EXTI_PR_PR7) != 0) {
        EXTI->IMR &= ~(EXTI_IMR_MR7);
    }

    if ((pr & EXTI_PR_PR8) != 0) {
        EXTI->IMR &= ~(EXTI_IMR_MR8);
    }

    if ((pr & EXTI_PR_PR9) != 0) {
        EXTI->IMR &= ~(EXTI_IMR_MR9);
    }

    NVIC_DisableIRQ(EXTI9_5_IRQn);

    EXTI->PR = EXTI_PR_PR5 | EXTI_PR_PR6 | EXTI_PR_PR7 | EXTI_PR_PR8 | EXTI_PR_PR9;

    OSAL_IRQ_EPILOGUE();
}

OSAL_IRQ_HANDLER(WB32_EXTI15_10_IRQ_VECTOR) {
    uint32_t pr;

    OSAL_IRQ_PROLOGUE();

    pr = EXTI->PR & (EXTI_PR_PR10 | EXTI_PR_PR11 | EXTI_PR_PR12 | EXTI_PR_PR13 | EXTI_PR_PR14 | EXTI_PR_PR15);

    if ((pr & EXTI_PR_PR10) != 0) {
        EXTI->IMR &= ~(EXTI_IMR_MR10);
    }

    if ((pr & EXTI_PR_PR11) != 0) {
        EXTI->IMR &= ~(EXTI_IMR_MR11);
    }

    if ((pr & EXTI_PR_PR12) != 0) {
        EXTI->IMR &= ~(EXTI_IMR_MR12);
    }

    if ((pr & EXTI_PR_PR13) != 0) {
        EXTI->IMR &= ~(EXTI_IMR_MR13);
    }

    if ((pr & EXTI_PR_PR14) != 0) {
        EXTI->IMR &= ~(EXTI_IMR_MR14);
    }

    if ((pr & EXTI_PR_PR15) != 0) {
        EXTI->IMR &= ~(EXTI_IMR_MR15);
    }

    NVIC_DisableIRQ(EXTI15_10_IRQn);

    EXTI->PR = EXTI_PR_PR10 | EXTI_PR_PR11 | EXTI_PR_PR12 | EXTI_PR_PR13 | EXTI_PR_PR14 | EXTI_PR_PR15;

    OSAL_IRQ_EPILOGUE();
}
/** @} */
