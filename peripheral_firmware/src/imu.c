#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/regulator.h>
#include <zephyr/sys/util.h>
#include <zephyr/logging/log.h>

#include "imu.h"

LOG_MODULE_REGISTER(imu, LOG_LEVEL_INF);

#define IMU_PWR_NODE DT_NODELABEL(lsm6ds3tr_c_en)
#define IMU_ODR_HZ   104

static const struct device *const imu_dev = DEVICE_DT_GET_ONE(st_lsm6dsl);

static int imu_power_on(void)
{
#if DT_NODE_EXISTS(IMU_PWR_NODE)
	const struct device *const imu_pwr = DEVICE_DT_GET(IMU_PWR_NODE);
	int ret;

	if (!device_is_ready(imu_pwr)) {
		LOG_ERR("regulator not ready");
		return -ENODEV;
	}

	ret = regulator_enable(imu_pwr);
	if (ret < 0) {
		LOG_ERR("regulator enable failed (%d)", ret);
		return ret;
	}

	k_msleep(20);
#endif
	return 0;
}

static int16_t clamp_i16(int64_t v)
{
	return (int16_t)CLAMP(v, INT16_MIN, INT16_MAX);
}

static int16_t accel_to_centi_ms2(const struct sensor_value *v)
{
	return clamp_i16(sensor_value_to_micro(v) / 10000);
}

static int16_t gyro_to_deci_dps(const struct sensor_value *v)
{
	return clamp_i16(sensor_value_to_micro(v) * 1800 / 3141593);
}

int imu_init(void)
{
	struct sensor_value odr = { .val1 = IMU_ODR_HZ, .val2 = 0 };
	int ret;

	ret = imu_power_on();
	if (ret < 0) {
		return ret;
	}

	ret = device_init(imu_dev);
	if (ret < 0 && ret != -EALREADY) {
		LOG_ERR("sensor init failed (%d)", ret);
		return ret;
	}

	if (!device_is_ready(imu_dev)) {
		LOG_ERR("sensor not ready");
		return -ENODEV;
	}

	ret = sensor_attr_set(imu_dev, SENSOR_CHAN_ACCEL_XYZ,
			      SENSOR_ATTR_SAMPLING_FREQUENCY, &odr);
	if (ret < 0) {
		LOG_ERR("accel odr failed (%d)", ret);
		return ret;
	}

	ret = sensor_attr_set(imu_dev, SENSOR_CHAN_GYRO_XYZ,
			      SENSOR_ATTR_SAMPLING_FREQUENCY, &odr);
	if (ret < 0) {
		LOG_ERR("gyro odr failed (%d)", ret);
		return ret;
	}

	LOG_INF("IMU ready at %d Hz", IMU_ODR_HZ);
	return 0;
}

int imu_read(struct imu_sample *out)
{
	struct sensor_value accel[3];
	struct sensor_value gyro[3];
	int ret;

	ret = sensor_sample_fetch(imu_dev);
	if (ret < 0) {
		return ret;
	}

	ret = sensor_channel_get(imu_dev, SENSOR_CHAN_ACCEL_XYZ, accel);
	if (ret < 0) {
		return ret;
	}

	ret = sensor_channel_get(imu_dev, SENSOR_CHAN_GYRO_XYZ, gyro);
	if (ret < 0) {
		return ret;
	}

	out->t_ms = k_uptime_get_32();
	out->ax = accel_to_centi_ms2(&accel[0]);
	out->ay = accel_to_centi_ms2(&accel[1]);
	out->az = accel_to_centi_ms2(&accel[2]);
	out->gx = gyro_to_deci_dps(&gyro[0]);
	out->gy = gyro_to_deci_dps(&gyro[1]);
	out->gz = gyro_to_deci_dps(&gyro[2]);

	return 0;
}
