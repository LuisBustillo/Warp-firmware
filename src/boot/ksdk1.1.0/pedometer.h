int16_t combine_stream(int16_t x_data, int16_t y_data, int16_t z_data);
void    lpf(void);
void    diff(void);

uint32_t countSteps(uint32_t step_count);
uint32_t countCals(uint32_t cal_count, uint8_t height, uint8_t weight);
uint8_t modeSelector(uint8_t mode, uint32_t last_step_time);

void displayBackground(uint8_t mode);
void displayMode(uint8_t mode);
void drawCount(uint8_t column, uint8_t row, uint32_t count, uint32_t colour);
void drawSteps(uint8_t step_count, uint8_t mode);
void drawCals(uint32_t cals, uint8_t mode);