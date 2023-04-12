#include "mbed.h"
#include <cstdint>
#include "TextLCD.h"

#define LOCATE 0xaa
#define CLS 0xbb
#define PUTC 0xcc


Thread thread_slave;

I2C i2c_lcd(D14, D15); // SDA, SCL
TextLCD_I2C lcd(&i2c_lcd, 0x4E, TextLCD::LCD16x2);

//master

SPI spi(D11, D12, D13); // mosi, miso, sclk
DigitalOut cs(D9);

SPISlave device(PD_4, PD_3, PD_1, PD_0); //mosi, miso, sclk, cs; PMOD pins

int commandGenerate(uint8_t a, uint8_t b)
{
    uint16_t cmd;
    cmd = b;
    cmd += a << 8;
    return cmd;
}

int slave()
{
    uint16_t storedValue[256];
    device.format(16, 3);
    device.frequency(1000000);
    //device.reply(0x00); // Prime SPI with first reply
    while (true){
        if (device.receive()){
            int cmd = device.read(); // Read byte from master
            uint8_t a = cmd >> 8;
            uint8_t b = cmd & 0xff;
            if (b == PUTC){
                int cmd = device.read(); // Read byte from master
                a = cmd >> 8;
                b = cmd & 0xff;
                lcd.putc(b);
                printf("Device: putc(%c).\n", b);
            }else if(b == LOCATE){
                int cmd = device.read(); // Read byte from master
                a = cmd >> 8;
                b = cmd & 0xff;
                lcd.locate(a, b);
                printf("Device: locate(%d, %d).\n", a, b);
            }else if(b == CLS){
                lcd.cls();
                printf("Device: cls().\n");
            }
        }
    }
}

int main()
{
    thread_slave.start(slave);
    lcd.setCursor(TextLCD::CurOff_BlkOn);
    int cmd;
    int response;
    spi.format(16, 3);
    spi.frequency(1000000);

    cs = 1;

    cs = 0;
    printf("Main: putc('x'). \n");
    cmd = commandGenerate(0, PUTC);
    spi.write(cmd);
    ThisThread::sleep_for(100ms);
    cmd = commandGenerate(0, 'x');
    spi.write(cmd);
    ThisThread::sleep_for(100ms);

    
}