#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/kernel.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#include "lbs.h"

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(bt_lbs, LOG_LEVEL_INF);

static bool notify_enabled;
static bool button_state;
static struct bt_lbs_cb lbs_cb;

static void lbslc_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
	notify_enabled = (value == BT_GATT_CCC_NOTIFY);
}

static ssize_t write_led(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			 const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
	if (len != 1U) {
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
	}
	if (offset != 0) {
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}
	if (lbs_cb.led_cb) {
		uint8_t val = *((uint8_t *)buf);
		if (val == 0x00 || val == 0x01) {
			lbs_cb.led_cb(val ? true : false);
		} else {
			return BT_GATT_ERR(BT_ATT_ERR_VALUE_NOT_ALLOWED);
		}
	}
	return len;
}

static ssize_t read_button(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			   void *buf, uint16_t len, uint16_t offset)
{
	const char *value = attr->user_data;

	if (lbs_cb.button_cb) {
		button_state = lbs_cb.button_cb();
		return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
					 sizeof(*value));
	}
	return 0;
}

/* LED Button Service: your routes table */
BT_GATT_SERVICE_DEFINE(lbs_svc,
	BT_GATT_PRIMARY_SERVICE(BT_UUID_LBS),                    /* attrs[0] */
	BT_GATT_CHARACTERISTIC(BT_UUID_LBS_BUTTON,               /* attrs[1] decl, [2] value */
			       BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
			       BT_GATT_PERM_READ, read_button, NULL,
			       &button_state),
	BT_GATT_CCC(lbslc_ccc_cfg_changed,                       /* attrs[3] */
		    BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
	BT_GATT_CHARACTERISTIC(BT_UUID_LBS_LED,                  /* attrs[4] decl, [5] value */
			       BT_GATT_CHRC_WRITE, BT_GATT_PERM_WRITE,
			       NULL, write_led, NULL),
	/* add new characteristics here, at the end */
);

int bt_lbs_init(struct bt_lbs_cb *callbacks)
{
	if (callbacks) {
		lbs_cb.led_cb    = callbacks->led_cb;
		lbs_cb.button_cb = callbacks->button_cb;
	}
	return 0;
}

int bt_lbs_send_button_state(bool button_state)
{
	if (!notify_enabled) {
		return -EACCES;
	}
	return bt_gatt_notify(NULL, &lbs_svc.attrs[2], &button_state,
			      sizeof(button_state));
}