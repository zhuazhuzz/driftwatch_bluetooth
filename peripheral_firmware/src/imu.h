#ifndef IMU_H_
#define IMU_H_

#include <zephyr/types.h>
#include <zephyr/toolchain.h>

struct imu_sample {
	uint32_t t_ms;
	int16_t ax;
	int16_t ay;
	int16_t az;
	int16_t gx;
	int16_t gy;
	int16_t gz;
} __packed;

int imu_init(void);

int imu_read(struct imu_sample *out);

#endif
