/*  main.c  - main */

#include <xinu.h>
#include <stdio.h>
#include <string.h>
int pin_led = 7;
int pin_adc = 1; //fixed in the ADC code
int pin_button = 1; //GPIO2_17, range of values 0-15
uid32 slotServer;
uid32 slotClient;

void operations() {
	//To obtain the system time
	int32	retval;			/* return value			*/
	uint32	now;			/* current local time		*/
	char	datestr[64];		/* printable date in ascii	*/

	retval = gettime(&now);
	ascdate(now, datestr);
	printf("%s\n", datestr);
     
	//UDP code PART 2
	//UDP Receive Data
	char recDataBuff[10];
	int recLen = sizeof(struct netpacket);
	int32 recv = udp_recv(
		slotClient,            /* Slot in table to use        */
		recDataBuff,            /* Buffer to hold UDP data    */
		recLen,            /* Length of buffer        */
		1000            /* Read timeout in msec        */
		);
        printf("Length of datagram received%d\n",recv);
        printf("Rec Data at indices :%c\n%c\n%c\n%c\n",recDataBuff[0],recDataBuff[1],recDataBuff[2],recDataBuff[3]);
	printf("Rec Data : %s \n", recDataBuff);


	//Todo the read/write operations 
	//ADC Temperature sensor block
	char data_adc[4]; //ADC data
	unsigned int integer = 0;
        unsigned int decimal = 0;
        float voltage;
	pread('A', pin_adc, (char*) data_adc);
        unsigned int vol = *(unsigned int*)data_adc;
        printf("voltage %d\n",vol);
	voltage = (vol*1.8/4095)*1000;
	float temp = (voltage - 500)/10 ; //considering that the voltage is in mVolts
	integer = (unsigned int)voltage;
	decimal = (unsigned int)((voltage - integer)*10);
	kprintf("voltage = %d.%d  mv\r\n",integer,decimal);		

	//Button block
	char seq_btn[4]; //Button state data
	pread('D', pin_button, (char*) seq_btn);
        unsigned int switch_state = *(unsigned int*)seq_btn;
        kprintf("switch state %d\n",switch_state);
	//int btn_val = *switch_state;

	//LED block        
        int led_in =0 ; //LED data
	if(recDataBuff[0]=='1') {
		led_in=1;
	}
        printf("LED value%d\n",led_in);
	pwrite('D', pin_led, led_in); // Turn on LED	

	//JSON String Generation

//   "led_1_status": "ON",
//   "led_2_status": "OFF",
//   "temperature": "35",
//   "time": "2016-11-25T15:10:19.336988",
//   "system_power": "ON"
	
	int t = (int) temp;
	char c0 = '0';
	char c1 = '0';
	if(t/100 == 0) {
		int t0 = t/10;
		c0 = t0 +'0';
		int i = t/10;
		int t1 = t-(i*10);
		c1 = t1 +'0';
	} else {
		kprintf("Temperature is greater than 2 digit\n");
	}

	char jsonStr[59]  = "{\"led_status\": \"0\",\"temperature\": \"35\",\"system_power\": \"1\"}";
	jsonStr[16] = led_in + '0'; //LED state
	jsonStr[35] = c0; //Temperature ten's digit
	jsonStr[36] = c1; //Temperature one's digit
	jsonStr[56] = switch_state + '0'; //Power Button state
	kprintf(" jsonStr : %s\n", jsonStr);

	//UDP code PART 3
	//UDP Send Data
	int sendLen = sizeof(jsonStr)/sizeof(jsonStr[0]);
	
	
	status status0 = udp_send (
		slotServer,            /* Table slot to use        */
		(char *)jsonStr,            /* Buffer of UDP data        */
		sendLen            /* Length of data in buffer    */
		);
	printf("Sent Data: %s \n", jsonStr);
}

int main(){
	while(1) {
	kprintf("In main. Before test function call");
	//UDP code PART 1
	//UDP Initialization
	udp_init();
	slotServer = udp_register (
		3232235778,//getlocalip(),            /* Remote IP address(server IP) or zero(if the board is acting as the server then no particular client address should be given) */
		3000,//22,//130,        /* Remote UDP protocol port    */
		5000//8888            /* Local UDP protocol port    */
		);
	printf("Slot of the Server : %d\n", slotServer);
	uint32 ip = getlocalip();
	printf("getlocalip : %x", ip);
	printf("getlocalip : %d", ip);

	slotClient = udp_register (
		0,//getlocalip(),            /* Remote IP address(server IP) or zero(if the board is acting as the server then no particular client address should be given) */
		0,//22,//129,//8888,        /* Remote UDP protocol port    */
		5000//4002            /* Local UDP protocol port    */
		);
	printf("Slot of the Client : %d\n", slotClient);
		operations();
		sleepms(1000);
	
	//UDP code PART 4
	//UDP Slot release
	udp_release(slotServer);
	udp_release(slotClient);
	}
    return 0;
}



