#include <stdio.h>
#include <stdint.h>

#define CALIBRATION_POINTS 5

// Enter values collected from the potentiometer,
// they must be [adc_value:calibrated_distance]
uint32_t x[CALIBRATION_POINTS] = {0, 235, 762, 1525, 3519};
uint32_t y[CALIBRATION_POINTS] = {0, 200, 650, 1300, 3000};

/*
    \brief Checks the linearity of the linear regression model.
    \param None
    \return r^2 correlation coefficient of the linear regression (0-100)
*/
uint32_t CheckLinearity(void) {
    uint64_t sum_x = 0;
    uint64_t sum_y = 0;
    uint64_t sum_xy = 0;
    uint64_t sum_x2 = 0;
    uint64_t sum_y2 = 0;

    for (size_t i = 0; i < CALIBRATION_POINTS; i++) {
        sum_x += x[i];
        sum_y += y[i];
        sum_xy += (uint64_t)x[i] * y[i];
        sum_x2 += (uint64_t)x[i] * x[i];
        sum_y2 += (uint64_t)y[i] * y[i];
    }

    int64_t num = (CALIBRATION_POINTS * sum_xy) - (sum_x * sum_y);
    int64_t den_x = (CALIBRATION_POINTS * sum_x2) - (sum_x * sum_x);
    int64_t den_y = (CALIBRATION_POINTS * sum_y2) - (sum_y * sum_y);
    int64_t den = den_x * den_y;

    if (den <= 0) {
        return 0;
    }

    return (uint32_t)((100 * (num * num)) / den);
}

int main(void) {
    printf("Correlation coefficient: %d\n", CheckLinearity());
    return 0;
}
