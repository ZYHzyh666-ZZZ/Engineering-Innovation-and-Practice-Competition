#ifndef _DRV_BIUBIU_H_
#define _DRV_BIUBIU_H_
#include "SysConfig.h"

#define ANO_RCC_LAZER_1 RCC_AHB1Periph_GPIOB
#define ANO_GPIO_LAZER_1 GPIOB
#define ANO_Pin_LAZER_1 GPIO_Pin_12

#define ANO_RCC_LAZER_2 RCC_AHB1Periph_GPIOA
#define ANO_GPIO_LAZER_2 GPIOA
#define ANO_Pin_LAZER_2 GPIO_Pin_6

#define LAZER_1_ON ANO_GPIO_LAZER_1->BSRRL = ANO_Pin_LAZER_1
#define LAZER_1_OFF ANO_GPIO_LAZER_1->BSRRH = ANO_Pin_LAZER_1
#define LAZER_2_ON ANO_GPIO_LAZER_2->BSRRL = ANO_Pin_LAZER_2
#define LAZER_2_OFF ANO_GPIO_LAZER_2->BSRRH = ANO_Pin_LAZER_2


void Dvr_LAZER_Init(void);
void MY_Lazer_1_updata(u8 dT_ms);
void MY_Lazer_1_ON(u16 times);

void MY_Lazer_2_updata(u8 dT_ms);
void MY_Lazer_2_ON(u16 times);

#endif
