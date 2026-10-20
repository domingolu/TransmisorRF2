/*
 * nRF24L01.h - Stub para la simulación en Wokwi
 *
 * El firmware real incluye este header (a través de ProtocoloRF.h), pero
 * no usa ningún registro del nRF24L01+: la radio no se simula en Wokwi.
 * Solo se mantiene el include para que el código compile sin cambios.
 */

#ifndef NRF24L01_H_
#define NRF24L01_H_

#include <stdint.h>

namespace nRF24L01 {

constexpr uint8_t CONFIG = 0x00;
constexpr uint8_t EN_RXADDR = 0x02;
constexpr uint8_t SETUP_AW = 0x03;
constexpr uint8_t RF_CH = 0x05;
constexpr uint8_t RF_SETUP = 0x06;
constexpr uint8_t STATUS = 0x07;
constexpr uint8_t RX_ADDR_P0 = 0x0A;
constexpr uint8_t TX_ADDR = 0x10;
constexpr uint8_t RX_PW_0 = 0x11;
constexpr uint8_t FIFO_STATUS = 0x17;

}  // namespace nRF24L01

#endif  // NRF24L01_H_
