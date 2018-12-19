/**
 * \file
 *
 * \brief Component description for LINCTRL
 *
 * Copyright (c) 2016 - 2017 Atmel Corporation,
 *                    a wholly owned subsidiary of Microchip Technology Inc.
 *
 * \asf_license_start
 *
 * \page License
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the Licence at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAMHA1_LINCTRL_COMPONENT_
#define _SAMHA1_LINCTRL_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR LINCTRL */
/* ========================================================================== */
/** \addtogroup SAMHA1_LINCTRL LIN control module */
/*@{*/

#define LINCTRL_U2255
#define REV_LINCTRL                 0x100

/* -------- LINCTRL_CTRL : (LINCTRL Offset: 0x0) (R/W 16) control register -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t WOCFG:2;          /*!< bit:  0.. 1  wkout observation select           */
    uint16_t :14;              /*!< bit:  2..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} LINCTRL_CTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define LINCTRL_CTRL_OFFSET         0x0          /**< \brief (LINCTRL_CTRL offset) control register */
#define LINCTRL_CTRL_RESETVALUE     _U_(0x0000)  /**< \brief (LINCTRL_CTRL reset_value) control register */

#define LINCTRL_CTRL_WOCFG_Pos      0            /**< \brief (LINCTRL_CTRL) wkout observation select */
#define LINCTRL_CTRL_WOCFG_Msk      (_U_(0x3) << LINCTRL_CTRL_WOCFG_Pos)
#define LINCTRL_CTRL_WOCFG(value)   (LINCTRL_CTRL_WOCFG_Msk & ((value) << LINCTRL_CTRL_WOCFG_Pos))
#define LINCTRL_CTRL_MASK           _U_(0x0003)  /**< \brief (LINCTRL_CTRL) MASK Register */

/** \brief LINCTRL hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO LINCTRL_CTRL_Type         CTRL;        /**< \brief Offset: 0x0 (R/W 16) control register */
} Linctrl;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMHA1_LINCTRL_COMPONENT_ */
