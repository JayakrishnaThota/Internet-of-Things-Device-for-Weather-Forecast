#include <xinu.h>

#define GPIO0_BASE 0x44E07000
#define GPIO1_BASE 0x4804C000
#define GPIO2_BASE 0x481AC000
#define GPIO3_BASE 0x481AE000
#define GPIO_SIZE  0x00000FFF

// OE: 0 is output, 1 is input
#define GPIO_OE 0x134
#define GPIO_DATAIN 0x138
#define GPIO_DATAOUT 0x13C
#define GPIO_DATACLEAROUT 0x190

#define USR0_LED (1<<21)
#define USR1_LED (1<<22)
#define USR2_LED (1<<4) //GPIO_68
#define USR3_LED (1<<24)

static void io_setup(void)
{
    
    kprintf("in io setup");  
	uint32 *creg = GPIO2_BASE+ GPIO_OE;
	kprintf("gpio_oe %d\n",creg);
    *creg = 0xFFFF0000; //Setting the higher half pins as input and the lower half pins as output
}

//To read the state of the button
int read_btn(int pinIndex, char *data_switch, int count)
{   
	//pinIndices : 16-31 , param value for index 0-15
    kprintf("in read_btn function\n"); 
    io_setup();
    // might have to remove the iterations 
		uint32 *gpio2 = GPIO2_BASE + GPIO_DATAIN;
		kprintf("address of datain for gpio_2: %d\n", gpio2);
        int32 val = 0;
        val = *gpio2;
		printf("val : %d \n",val);
                printf("val pinIndex%d\n",( val >> ( pinIndex + 15 )));
//param value pinIndex in the range 0-15
        uint32 d = ( val >> ( pinIndex + 15)) & 0x1;
        kprintf("pin val : %d\n", d);
		if(d) {
                       kprintf("if called");
			*data_switch = 1; 
		} else {
                       kprintf("else called");
			*data_switch = 0;
		}
        kprintf("data switch value%d\n",*data_switch);
       // sleepms(2000); // May be need to comment this out, in case the loop is removed
    

    return 0;
}

//To read the state of the button
int write_LED(int pinIndex, int data_buffer) // We can change the data to bool, if it is convenient?
{   
	//pinIndices : 0 - 15
    kprintf("in write_LED function"); 
    io_setup();
/*    while (1) {*/ // might have to remove the iterations
		int pinData = data_buffer;
                printf("pin Data : %d\n",pinData);
                //kprintf("address of dataout for gpio_2: %d\n", gpio2);
		//kprintf("Value of the pins %d\n",val);
                if(pinData==1)
                {
			uint32 *gpio2_dataout = GPIO2_BASE + GPIO_DATAOUT;
			int32 val = *gpio2_dataout; // Get existing copy of the pins
		        val = (pinData<<pinIndex) | val;
		        printf("val of the pins bit shifted : %d \n",val);
			*gpio2_dataout = val; // write the modified value to the dataout register again
		        printf("val of gpio %d\n",*gpio2_dataout);
		}
                else
               {
		       uint32 *gpio2_dataclearout = GPIO2_BASE + GPIO_DATACLEAROUT;
                       *gpio2_dataclearout =  (1<<pinIndex);
                        printf("value in clearout %d\n",*gpio2_dataclearout);
			//uint32 *gpio2_dataout = GPIO2_BASE + GPIO_DATAOUT;
			//int32 val = *gpio2_dataout; // Get existing copy of the pins
		       // val = (0xFFFFFFFE<<pinIndex) | (0xFFFFFFFE>>(32-pinIndex));
                        //printf("value of the pins %d\n",
		        //printf("val of the pins bit shifted : %d \n",val);
			//*gpio2_dataout = val; // write the modified value to the dataout register again
		        //printf("val of gpio %d\n",*gpio2_dataout);
	       }		
        //sleepms(2000); // May be need to comment this out, in case the loop is removed
    /*}*/

    return 0;
}

/*int main(void)
{   
    while (1) {
  
     char seq[4]="";

		switch(0) {

		   case 0  :
		      read_btn(1,seq,4);
		      break; 
	
		  case 1  :
		      write_LED(7,seq,4);
		      break; 
		  
		  
		   default :  
		   kprintf("dying");
		}

}
       

    return 0;
}*/
