#include <linux/module.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/of.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>

/* Private device structure */
struct lcd_data {
    struct i2c_client *client;
    struct miscdevice lcd_miscdevice;
    const char * name ; 
};
/*
* Read data by user 
*/
static ssize_t lcd_read_file(struct file *file, char __user *userbuf,size_t count, loff_t *ppos) {
    int expval, size;
    char buf[3];
    struct lcd_data *lcd;
    lcd = container_of(file->private_data, struct lcd_data, lcd_miscdevice);
    /* Store IO expander input in expval variable */
    expval = i2c_smbus_read_byte(lcd->client);
    if (expval < 0)
    return -EFAULT;
    /*
    * Convert expval int value into a char string.
    * For example, 255 int (4 bytes) = FF (2 bytes) + '\0' (1 byte) string.
    */
    size = sprintf(buf, "%02x", expval); /* size is 2 */
    /*
    * Replace NULL by \n. It is not needed to have a char array
    * ended with \0 character.
    */
    buf[size] = '\n';
    /* Send size+1 to include the \n character */
    if(*ppos == 0) {
        if(copy_to_user(userbuf, buf, size+1)) {
            pr_info("Failed to return led_value to user space\n");
            return -EFAULT;
        }
        *ppos+=1;
        return size+1;
    }
    return 0;
}

static void lcd_send_data(struct i2c_client *client, u8 data);

/* 
* Write function 
*/
static ssize_t lcd_write_file(struct file *file, const char __user *userbuf,size_t count, loff_t *ppos) {
    int i;    
    char buf[100];
    struct lcd_data * lcd;
    lcd = container_of(file->private_data, struct lcd_data, lcd_miscdevice);    
    dev_info(&lcd->client->dev,"lcd_write_file entered on %s\n", lcd->name);
    if(copy_from_user(buf, userbuf, count)) {
        dev_err(&lcd->client->dev, "Bad copied value\n");
		return -EFAULT;
    }
    buf[count-1] = '\0'; /* replace \n with \0 */
    /* Convert the string to an unsigned long */
    for (i = 0; i < count - 1; i++)
      lcd_send_data(lcd->client, buf[i]);    
    return count;
}

static const struct file_operations lcd_fops = {
    .owner = THIS_MODULE,
    .read = lcd_read_file,
    .write = lcd_write_file,
};
/*
*/
static void lcd_init(struct i2c_client *client);

/* The probe() function is called after binding */
static int lcd_probe(struct i2c_client * client,const struct i2c_device_id * id) {    
    struct lcd_data * lcd;
    dev_info(&client->dev,"Probe entered...\n");
    /* Allocate a private structure */
    lcd = devm_kzalloc(&client->dev, sizeof(struct lcd_data), GFP_KERNEL);
    /* Store pointer to the device-structure in bus device context */
    i2c_set_clientdata(client,lcd);
    /* Store pointer to the I2C client device in the private structure */
    lcd->client = client;
    of_property_read_string(client->dev.of_node, "label", &lcd->name);    
    lcd->lcd_miscdevice.name = lcd->name;
    lcd->lcd_miscdevice.minor = MISC_DYNAMIC_MINOR;
    lcd->lcd_miscdevice.fops = &lcd_fops;
    /* Register misc device */
    misc_register(&lcd->lcd_miscdevice);        
    /* Initialize the LCD chip */
    lcd_init(client);
    return 0;
}

static void lcd_remove(struct i2c_client * client)
{
    struct lcd_data * lcd;
    /* Get device structure from bus device context */
    lcd = i2c_get_clientdata(client);
    dev_info(&client->dev,"lcd_remove is entered on %s\n", lcd->name);
    /* Deregister misc device */
    misc_deregister(&lcd->lcd_miscdevice);
}

static const struct of_device_id lcd_dt_ids[] = {
    {.compatible = "training,lcd2x16", },
    {}
};
MODULE_DEVICE_TABLE(of, lcd_dt_ids);

static const struct i2c_device_id i2c_ids[] = {
    {.name = "lcd2x16", },
    {}
};

MODULE_DEVICE_TABLE(i2c, i2c_ids);
static struct i2c_driver lcd_driver = {
    .driver = {
        .name = "lcd2x16",
        .owner = THIS_MODULE,
        .of_match_table = lcd_dt_ids,
    },
    .probe = lcd_probe,
    .remove = lcd_remove,
    .id_table = i2c_ids,
};
/***************************************************************************/
//##################################################################################################
#define LCD_REG_MAN_ID			0x27
#define LCD_REG_CHIP_ID			0xFF
//##################################################################################################
#define LCD_BL					0x08
#define LCD_EN					0x04  // Enable bit
#define LCD_RW					0x02  // Read/Write bit
#define LCD_RS					0x01  // Register select bit
#define LCD_MASK_DISABLE_EN		0xFB  // MASK of control

//##################################################################################################
//# LCD Commands:
//##################################################################################################
#define LCD_CMD_CLEARDISPLAY    0x01
#define LCD_CMD_RETURNHOME      0x02
#define LCD_CMD_ENTRYMODESET    0x04
#define LCD_CMD_DISPLAYCONTROL  0x08
#define LCD_CMD_CURSORSHIFT     0x10
#define LCD_CMD_FUNCTIONSET     0x20
#define LCD_CMD_SETCGRAMADDR    0x40
#define LCD_CMD_SETDDRAMADDR    0x80
#define LCD_CLEARDISPLAY    	0x01
#define LCD_RETURNHOME      	0x02
#define LCD_ENTRYMODESET    	0x04
#define LCD_DISPLAYCONTROL  	0x08
#define LCD_CURSORSHIFT     	0x10
#define LCD_FUNCTIONSET     	0x20
#define LCD_SETCGRAMADDR    	0x40
#define LCD_CTRL_BLINK_ON	0x01
#define LCD_CTRL_BLINK_OFF	0x00
#define LCD_CTRL_DISPLAY_ON	0x04
#define LCD_CTRL_DISPLAY_OFF	0x00
#define LCD_CTRL_CURSOR_ON	0x02
#define LCD_CTRL_CURSOR_OFF	0x00
#define LCD_DISPLAYON   	0x04
#define LCD_DISPLAYOFF  	0x00
#define LCD_CURSORON    	0x02
#define LCD_CURSOROFF   	0x00
#define LCD_BLINKON     	0x01
#define LCD_BLINKOFF    	0x00
#define LCD_ENTRYRIGHT           0x00
#define LCD_ENTRYLEFT            0x02
#define LCD_ENTRYSHIFTINCREMENT  0x01
#define LCD_ENTRYSHIFTDECREMENT  0x00
#define LCD_DISPLAYMOVE  	0x08
#define LCD_CURSORMOVE   	0x00
#define LCD_MOVERIGHT    	0x04
#define LCD_MOVELEFT     	0x00
#define LCD_8BITMODE  		0x10
#define LCD_4BITMODE  		0x00
#define LCD_2LINE     		0x08
#define LCD_1LINE     		0x00
#define LCD_5x10DOTS  		0x04
#define LCD_5x8DOTS   		0x00
//##################################################################################################
/**   Function:	static int lcd_i2c_write_byte(struct i2c_client *client, u8 *data)
 **   Desc:	This function will go write a byte in i2c protocol.
 */
static int lcd_i2c_write_byte(struct i2c_client *client, u8 *data) {
  int ret;
  ret = i2c_master_send(client, data, 1);
  if (ret < 0)
    dev_warn(&client->dev, "Write byte in i2c ['0x%02X'] failed.\n", ((int)*data));
  return ret;
}
/*
* Read data
*/
static int lcd_i2c_read_byte(struct i2c_client *client) {
  u8 i2c_data[1];
  int ret = 0;
  ret = i2c_master_recv(client, i2c_data, 1);
  if (ret < 0) {
    dev_warn(&client->dev, "i2c read data failed\n");
    return ret;
  }
  return (i2c_data[0]);
}
/*
*/
bool backlightFlag = true;
static void lcd_en_strobe(struct i2c_client *client) {  
  u8 lcd_data;
  lcd_data = lcd_i2c_read_byte(client);
  lcd_data = (lcd_data | LCD_EN | (backlightFlag == 1 ? LCD_BL : 0x00));
  lcd_i2c_write_byte(client, &lcd_data);
  ndelay(1);	
  lcd_data &= LCD_MASK_DISABLE_EN | (backlightFlag == 1 ? LCD_BL : 0x00);
  lcd_i2c_write_byte(client, &lcd_data);
}

static void lcd_write_4bits(struct i2c_client *client, u8 value)  {  
  u8 data[1];
  data[0]=value | LCD_EN;
  lcd_i2c_write_byte(client,data);
  msleep(1);
  data[0]=(u8)(value & (u8)~LCD_EN);
  lcd_i2c_write_byte(client,data);
  msleep(50);
}

static void lcd_send_byte(struct i2c_client *client, u8 msb, u8 lsb)  {
    if (backlightFlag) {
        lsb = lsb | LCD_BL;
        msb = msb | LCD_BL;
    }
    lcd_write_4bits(client,msb);
    lcd_write_4bits(client,lsb);
}

/*
* Send command
*/
static void  lcd_send_cmd(struct i2c_client *client, u8 cmd) {
    u8 MSN = (cmd >> 4) & 0x0F;
    u8 LSN = cmd & 0x0F;
    u8 MSb = MSN << 4;
    u8 LSb = LSN << 4;
    lcd_send_byte(client,MSb, LSb);
}

/*
* Set cursor position
*/
static void lcd_set_position(struct i2c_client *client, int line, int col)
{
  u8 lcd_row_offset[4] = {0x80, 0xC0, 0x14, 0x54};
  lcd_send_cmd(client,  (lcd_row_offset[line] + col)); 
}

/**    
 **   Desc:
 */
static void lcd_send_data(struct i2c_client *client, u8 data)
{
  u8 d;   
  d = (data & 0xF0) | LCD_RS | (backlightFlag == 1 ? LCD_BL : 0x00);
  lcd_i2c_write_byte(client, &d);
  lcd_en_strobe(client);
  d = (data << 4) | LCD_RS | (backlightFlag == 1 ? LCD_BL : 0x00);
  lcd_i2c_write_byte(client, &d);
  lcd_en_strobe(client);   
}

static void lcd_put_char(struct i2c_client *client,u8 bits)  {
    u8 MSN = bits  & 0xF0;
    u8 LSN = bits & 0x0F;
    u8 MSb = MSN  | LCD_RS;
    u8 LSb = (LSN << 4) | LCD_RS;
    lcd_send_byte(client,MSb, LSb);
}
/*
* Display a string
*/
static void lcd_puts(struct i2c_client *client, const char *string, u8 line, u8 col, u8 count)
{
  u8 i;
  lcd_set_position(client, line, col);
  for (i = 0; i < count - 1; i++)  
    lcd_send_data(client, string[i]);  
}
/*
* Initialize the lcd
*/
static void lcd_init(struct i2c_client *client)
{
  u8 msg[15] = {"CARREGANDO ..."};
  int displayshift = (LCD_CURSORMOVE | LCD_MOVERIGHT);
  int displaymode = (LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT);
  int displaycontrol = (LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF);
  // Initialization
  lcd_send_cmd(client, 0x02);
  lcd_send_cmd(client, LCD_FUNCTIONSET | LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS);
  lcd_send_cmd(client, LCD_DISPLAYCONTROL | displaycontrol);
  lcd_send_cmd(client, LCD_ENTRYMODESET | displaymode);
  lcd_send_cmd(client, LCD_CLEARDISPLAY);
  lcd_send_cmd(client, LCD_CURSORSHIFT | displayshift);  
  lcd_send_cmd(client, LCD_RETURNHOME);
  lcd_puts(client, msg, 0, 0, 15);  
}
module_i2c_driver(lcd_driver);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("C.A. ABID");
MODULE_DESCRIPTION("This is a client i2c driver that control a LCD display connected via i2c bus");
