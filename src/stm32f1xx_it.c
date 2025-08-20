/* stm32f1xx_it.c
 * Interrupt handlers used by the project. Provides EXTI handlers for
 * user keys on PB1 and PB11. The actual button handling logic should
 * be placed into HAL_GPIO_EXTI_Callback() in application code.
 */

#include "board.h"
#include "stm32f1xx_hal.h"
extern TIM_HandleTypeDef htim2;

extern void Error_Handler(void);

/* Cortex-M3 Processor Exceptions Handlers (minimal implementations) */
void NMI_Handler(void)
{
  /* NMI handler: nothing to do */
}

void HardFault_Handler(void)
{
  /* On hard fault, forward to project error handler which halts the CPU */
  Error_Handler();
}

void MemManage_Handler(void)
{
  Error_Handler();
}

void BusFault_Handler(void)
{
  Error_Handler();
}

void UsageFault_Handler(void)
{
  Error_Handler();
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

/* External interrupt handlers for user keys
 * - PB1  -> EXTI line 1  -> EXTI1_IRQHandler
 * - PB11 -> EXTI line 11 -> EXTI15_10_IRQHandler
 *
 * The handlers call HAL_GPIO_EXTI_IRQHandler(pin) which clears the
 * EXTI pending bit and forwards the event to HAL_GPIO_EXTI_Callback(pin)
 * where user code should handle the button press.
 */
void EXTI1_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

void EXTI15_10_IRQHandler(void)
{
  /* Handle PB11 (GPIO_PIN_11). If you add more EXTI lines in 10..15
     range, call HAL_GPIO_EXTI_IRQHandler() for those pins as well. */
  /* Forward EXTI lines in 10..15 range. PB11 and PB14 are used by this
    project (buttons on PB11, IR sensor on PB14). Call the HAL helper
    for each pin so HAL_GPIO_EXTI_Callback() receives the event. */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
}

void TIM2_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim2);
}

/* NOTE: Implement HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) in your
 * application (for example in src/main.c or src/board.c) to react to
 * button presses. Example signature:
 *
 * void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
 * {
 *   if (GPIO_Pin == GPIO_PIN_1) { ... }
 *   else if (GPIO_Pin == GPIO_PIN_11) { ... }
 * }
 */
