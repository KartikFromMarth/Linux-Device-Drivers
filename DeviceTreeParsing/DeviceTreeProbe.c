#include <linux/module.h>
#include <linux/init.h>
#include <linux/mod_devicetable.h>
#include <linux/property.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>



#define DRIVER_NAME "my_device_driver"

struct my_device_data {
    int my_value;
    const char *label;
    const char *status;
};



static int my_device_probe(struct platform_device *pdev) 
{

  struct device_node *np = pdev->dev.of_node;
  struct my_device_data *data;
  int ret;

  data = devm_kzalloc(&pdev->dev, sizeof(*data), GFP_KERNEL);
  if (!data)
  {
	dev_err(&pdev->dev, "Failed to allocate memory\n");
	return -ENOMEM;
  }

    // Read value
    ret = of_property_read_u32(np, "my_value", &data->my_value);
    if (ret) {
        dev_err(&pdev->dev, "Failed to read my_value property\n");
        return ret;
    }

    // Read label
    ret = of_property_read_string(np, "label", &data->label);
    if (ret) {
        dev_err(&pdev->dev, "Failed to read label property\n");
        return ret;
    }

    // Read status
    ret = of_property_read_string(np, "status", &data->status);
    if (ret) {
        dev_err(&pdev->dev, "Failed to read status property\n");
        return ret;
    }

   dev_info(&pdev->dev, "my_value: %d\n", data->my_value);
   dev_info(&pdev->dev, "label: %s\n", data->label);
   dev_info(&pdev->dev, "status: %s\n", data->status);
   

    platform_set_drvdata(pdev, data);

    return 0;

}



static int my_device_remove(struct platform_device *pdev) {
    dev_info(&pdev->dev, "Removing my_device\n");
    return 0;

}


static const struct of_device_id my_device_of_match[] = {
    { .compatible = "happy", },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, my_device_of_match);

static struct platform_driver my_device_driver = {
    .driver = {
        .name = DRIVER_NAME,
        .of_match_table = my_device_of_match,
    },
    .probe = my_device_probe,
    .remove = my_device_remove,
};






module_platform_driver(my_device_driver);


/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kartik");
MODULE_DESCRIPTION("A simple LKM to parse the device tree for a specific device and its properties");
