#ifndef BITS_H
#define BITS_H

#include <stdint.h>
#include <stdbool.h>

/* 
    \brief Subroutine to return only bits PF4, PF1 and PF0 of GPIO_PORTF_DATA_R if they were toggled.

    \param data_reg Current GPIO_PORTF_DATA_R passed value
    \param before_toggle GPIO_PORTF_DATA_R value before the toggling of any bits
    \return Bits that have toggled between the two values. '1' indicates a bit has toggled.
    \note Think how one bitwise operation can help check whether it toggled or not.
*/
uint32_t ToggledBits(uint32_t data_reg, uint32_t before_toggle);

#endif /* BITS_H */
