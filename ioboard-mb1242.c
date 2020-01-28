//[*]------------------------------------------------------------------------[*]
//
//
//  ODROID IOBOARD Board : IOBOARD MB1242 Sensor driver
//  Rommel Samanez
//  2020.01.27
//
//
//[*]------------------------------------------------------------------------[*]
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/input-polldev.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/workqueue.h>

//[*]------------------------------------------------------------------------[*]
#define MB1242_DRV_NAME	"ioboard-mb1242"

#define MB1242_WORK_PERIOD  msecs_to_jiffies(1000)

#define MB1242_READING_DELAY	100         // Miliseconds

//[*]------------------------------------------------------------------------[*]
// Register definitions
//[*]------------------------------------------------------------------------[*]
#define MB1242_TAKE_MEAS_REG    0x51


//[*]------------------------------------------------------------------------[*]
//
// Driver private data
//
//[*]------------------------------------------------------------------------[*]
struct mb1242_data {
	struct i2c_client	*client;
	struct delayed_work	work;
	bool			enabled;
	unsigned int	distance;
};
//[*]------------------------------------------------------------------------[*]
//
// Device dependant operations
//
//[*]------------------------------------------------------------------------[*]
static int mb1242_i2c_read(const struct i2c_client *client, u8 cmd, u8 *buf, int len)
{
	int err;

	err = i2c_smbus_read_i2c_block_data(client, cmd, len, buf);

	if (err == len)
		return 0;

	return err;
}

//[*]------------------------------------------------------------------------[*]
static int mb1242_i2c_write(const struct i2c_client *client, u8 cmd)
{
	int err;
	err = i2c_smbus_write_byte(client, cmd);
	if (!err)
		return 0;

	return err;
}

//[*]------------------------------------------------------------------------[*]
static int mb1242_get_raw_distance(struct mb1242_data *mb1242, u16 *raw_distance)
{
	int err;
	u16 buf;

	err = mb1242_i2c_write(mb1242->client, MB1242_TAKE_MEAS_REG);
	if (err) {
		pr_err("%s: can't write MB1242_TAKE_MEAS_REG\n", __func__);
		return err;
	}

	msleep(MB1242_READING_DELAY);

	err = mb1242_i2c_read(mb1242->client, 0, (u8 *)&buf, 2);
	if (err) {
		pr_err("%s: Fail to read distance value\n", __func__);
		return err;
	}

	*raw_distance = be16_to_cpu(buf);

	return err;
}


//[*]------------------------------------------------------------------------[*]
static void mb1242_work_func(struct work_struct *work)
{
	u16	raw_distance;

	struct mb1242_data *mb1242 = container_of((struct delayed_work *)work,
							struct mb1242_data, work);

	if (mb1242_get_raw_distance(mb1242, &raw_distance)) {
		pr_err("%s: can't read distance value\n", __func__);
		return;
	}

	mb1242->distance = raw_distance;

	if (mb1242->enabled)
		schedule_delayed_work(&mb1242->work, MB1242_WORK_PERIOD);
	else {
		mb1242->distance    = 0;
	}
}


//[*]------------------------------------------------------------------------[*]
//
// sysfs device attributes
// 
//[*]------------------------------------------------------------------------[*]
static ssize_t mb1242_distance_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct mb1242_data *mb1242 = dev_get_drvdata(dev);

	return sprintf(buf, "%d\n", mb1242->distance);
}


//[*]------------------------------------------------------------------------[*]
static 	ssize_t mb1242_enable_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct mb1242_data *mb1242 = dev_get_drvdata(dev);

	return sprintf(buf, "%d\n", mb1242->enabled);
}

//[*]------------------------------------------------------------------------[*]
static 	ssize_t mb1242_enable_set(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct mb1242_data	*mb1242 = dev_get_drvdata(dev);
	unsigned int		val;

	if (!(sscanf(buf, "%d\n", &val)))
		return -EINVAL;

	val = (val > 0) ? 1 : 0;

	if (mb1242->enabled != val) {
		mb1242->enabled = val;
		if (mb1242->enabled)
			schedule_delayed_work(&mb1242->work, MB1242_WORK_PERIOD);
	}

	return count;
}

//[*]------------------------------------------------------------------------[*]
static DEVICE_ATTR(distance,    0660, mb1242_distance_show,	NULL);
static DEVICE_ATTR(enable,      0660, mb1242_enable_show, mb1242_enable_set);

static struct attribute *mb1242_attributes[] = {
	&dev_attr_distance.attr,
	&dev_attr_enable.attr,
	NULL
};

static struct attribute_group mb1242_attribute_group = {
	.attrs = mb1242_attributes
};

//[*]------------------------------------------------------------------------[*]
//
// I2C client
//
//[*]------------------------------------------------------------------------[*]
static int mb1242_detect(struct mb1242_data *mb1242)
{
    u16	raw_distance;
	if (mb1242_get_raw_distance(mb1242, &raw_distance)) {
		return -1;
	}
    return 0;
}

//[*]------------------------------------------------------------------------[*]
static int mb1242_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct mb1242_data	*mb1242;
	int			err=0;

	mb1242 = kzalloc(sizeof(struct mb1242_data), GFP_KERNEL);
	if (!mb1242) {
		pr_err("%s: failed to allocate memory for module\n", __func__);
		return -ENOMEM;
	}

	i2c_set_clientdata(client, mb1242);

	dev_set_drvdata(&client->dev, mb1242);

	mb1242->client = client;

	if (mb1242_detect(mb1242) < 0)
		goto error;

	INIT_DELAYED_WORK(&mb1242->work, mb1242_work_func);

	if(mb1242->enabled) schedule_delayed_work(&mb1242->work, MB1242_WORK_PERIOD);

	if ((err = sysfs_create_group(&client->dev.kobj, &mb1242_attribute_group)) < 0)
		goto error;

	printk("\n=================== ioboard_%s ===================\n\n", __func__);

	return 0;

error:
	printk("\n=================== ioboard_%s FAIL! ===================\n\n", __func__);

	return err;
}

//[*]------------------------------------------------------------------------[*]
static int mb1242_remove(struct i2c_client *client)
{
	struct mb1242_data *mb1242 = i2c_get_clientdata(client);

	if (mb1242->enabled)
		cancel_delayed_work_sync(&mb1242->work);

	sysfs_remove_group(&client->dev.kobj, &mb1242_attribute_group);

	kfree(mb1242);
	return 0;
}

//[*]------------------------------------------------------------------------[*]
static const struct i2c_device_id mb1242_id[] = {
	{MB1242_DRV_NAME, 0},
	{},
};

MODULE_DEVICE_TABLE(i2c, mb1242_id);

//[*]------------------------------------------------------------------------[*]
struct i2c_driver mb1242_driver = {
	.driver = {
		.name	= MB1242_DRV_NAME,
		.owner	= THIS_MODULE,
	},
	.probe		= mb1242_probe,
	.remove		= mb1242_remove,
	.id_table	= mb1242_id,
};

//[*]------------------------------------------------------------------------[*]
/*
 * Module init and exit
 */
//[*]------------------------------------------------------------------------[*]
static int __init mb1242_init(void)
{
	return i2c_add_driver(&mb1242_driver);
}
module_init(mb1242_init);

//[*]------------------------------------------------------------------------[*]
static void __exit mb1242_exit(void)
{
	i2c_del_driver(&mb1242_driver);
}
module_exit(mb1242_exit);

//[*]------------------------------------------------------------------------[*]
MODULE_DESCRIPTION("IOBOARD driver for ODROIDXU-Dev board");
MODULE_AUTHOR("Rommel Samanez");
MODULE_LICENSE("GPL");

//[*]------------------------------------------------------------------------[*]
//[*]------------------------------------------------------------------------[*]
