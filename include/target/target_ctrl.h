#ifndef TARGET_CTRL_H
#define TARGET_CTRL_H

// Target RUN is active-low and is wired to GP22 (board_config.h).
// These functions currently execute on Core 0.
void target_ctrl_init(void);
void target_reset_low(void);   // Assert reset: RUN = 0
void target_reset_high(void);  // Release reset: RUN = 1

#endif
