/*
result
0x80000000 00:00.0 Class 0600: Device 8086:0f00 (rev: 0c)
0x80001000 00:02.0 Class 0300: Device 8086:0f31 (rev: 0c)
0x80009800 00:13.0 Class 0101: Device 8086:0f21 (rev: 0c)
0x8000d000 00:1a.0 Class 1080: Device 8086:0f18 (rev: 0c)
0x8000d800 00:1b.0 Class 0403: Device 8086:0f04 (rev: 0c)
0x8000e000 00:1c.0 Class 0604: Device 8086:0f48 (rev: 0c)
0x8000e100 00:1c.1 Class 0604: Device 8086:0f4a (rev: 0c)
0x8000e200 00:1c.2 Class 0604: Device 8086:0f4c (rev: 0c)
0x8000e300 00:1c.3 Class 0604: Device 8086:0f4e (rev: 0c)
0x8000e800 00:1d.0 Class 0c03: Device 8086:0f34 (rev: 0c)
0x8000f800 00:1f.0 Class 0601: Device 8086:0f1c (rev: 0c)
0x8000fb00 00:1f.3 Class 0c05: Device 8086:0f12 (rev: 0c)
0x80010000 01:00.0 Class 0200: Device 8086:1539 (rev: 03)
0x80020000 02:00.0 Class 0200: Device 8086:1539 (rev: 03)
*/

#include <inttypes.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/io.h> // inb/outb,etc...

#define PCI_CONFIG_ADDR 0xCF8
#define PCI_CONFIG_DATA 0xCFC

int pci_autodetect()
{
    uint32_t result;
    uint32_t io_addr;
    int function = 0;
    int bus = 0;
    int device = 0;
    int rev = 0;
    uint32_t vendor_id, dev_id, class1, class2, class3;

	for (bus = 0; bus<5; bus++)
	{
		for (device = 0; device<32; device++)
		{
			for(function = 0; function<8 ; function++ )
			{
                // 格式 31 Enabled 23:16 总线编号  15:11 设备编号 10:8 功能编号 7:2 配置空间寄存器编号 1:0 为0
                io_addr = 0x80000000 + (bus<<16) + (device<<11) + (function<<8);
                // 将地址写到CONFIG_ADDR寄存器
				outl(io_addr, PCI_CONFIG_ADDR);
				result = inl(PCI_CONFIG_DATA); // 读data数据寄存器，得到id
                if (result != 0xffffffff) // not the bad one print it
                {
                    vendor_id = result & 0xFFFF;
                    dev_id = result >> 16;
                    outl(io_addr+0x08, PCI_CONFIG_ADDR);
                    result = inl(PCI_CONFIG_DATA);
                    rev = result&0xff;
                    class3 = (result>>8)&0xff;
                    class2 = (result>>16)&0xff;
                    class1 = (result>>24)&0xff;
                    printf("0x%08x %02x:%02x.%x ", io_addr, bus, device, function);
                    //printf("PCI Read result: 0x%08x ioaddr: 0x%08x ", result, io_addr);
                    printf("Class %02x%02x: Device %04x:%04x (rev: %02x)\n", class1, class2, 
                                vendor_id, dev_id,rev);
                }
            }
        }
	}
	return 0;
}

int main(void)
{
    iopl(3);
    pci_autodetect();
    return 0;
}