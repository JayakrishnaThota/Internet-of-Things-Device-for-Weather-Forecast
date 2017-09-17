#include <xinu.h>
int pread(char dev_type, int dev_id, char *data_buffer){  //Device type ( ADC or GPIO), pin index, pointer to data buffer
kprintf("In pread. Dev_Type: %c, dev_id : %d, data_buffer : %s \n", dev_type, dev_id, data_buffer);
	switch (dev_type)  {
case 'D' :
read_btn(dev_id,(char*)data_buffer, 4);  //Pin index, pointer to data buffer, buffer size
break;
case 'A' :
read(ADC,(char*)data_buffer,4); //ADC type, pointer to data buffer, buffer size
break;
default: 
kprintf("No such type of Device to read");
 }
return 0;
}
int pwrite(char dev_type, int dev_id, int data_buffer){  //Device type ( ADC or GPIO), pin index, pointer to data buffer
kprintf("In pwrite. Dev_Type: %c, dev_id : %d, data_buffer : %d \n", dev_type, dev_id, data_buffer);
switch (dev_type)  {
case 'D' :
write_LED(dev_id, data_buffer); //Pin index, pointer to data buffer, buffer size
break;
default: 
kprintf("No such type of Device to write to");
break;
 }
return 0;
}
