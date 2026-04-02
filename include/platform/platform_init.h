#ifndef PLATFORM_INIT_H
#define PLATFORM_INIT_H

#include <stdint.h>
#include <stdbool.h>

/**
 * platform_init.h
 *
 * Interface da camada de inicialização da sonda.
 * Responsável por clocks, GPIOs e estado inicial seguro.
 *
 * Deve ser chamada uma única vez, no início do main(),
 * antes de qualquer outro módulo.
 */

/**
 * @brief Inicializa a plataforma completa da sonda.
 *
 * Realiza, em ordem:
 *   1. Inicialização do Pico SDK (stdio, clocks)
 *   2. Configuração dos GPIOs de controle (LED, RUN do alvo)
 *   3. Estado inicial seguro: alvo mantido em reset, LED apagado
 *
 * @return true  se a inicialização foi bem-sucedida
 * @return false se alguma etapa falhou (reservado para extensões futuras)
 */
bool platform_init(void);

/**
 * @brief Acende o LED onboard da sonda.
 */
void platform_led_on(void);

/**
 * @brief Apaga o LED onboard da sonda.
 */
void platform_led_off(void);

/**
 * @brief Pisca o LED n vezes com período fixo (bloqueante).
 *
 * Útil para sinalização de estado durante boot ou erro.
 *
 * @param n       número de piscadas
 * @param period_ms duração de cada ciclo em milissegundos
 */
void platform_led_blink(uint8_t n, uint32_t period_ms);

#endif /* PLATFORM_INIT_H */