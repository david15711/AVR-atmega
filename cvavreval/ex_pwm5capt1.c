// PWM CTC를 CAPT Test 
// CDSP.MJU.KR (241118 Class) 
#include <mega128.h>
#include <delay.h>    

#define DSTop 4000 // TopValue 지정

#define Sw2on ( PING & 0x04 ) // 스위치 On 0x04, Off 0x00 
#define Sw3on ( PING & 0x08 ) // 스위치 On 0x08, Off 0x00
       //PB5 to PE7 결선
unsigned char seg0[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7c,0x07,0x7f,0x67}; // 7-segment set 0~9
unsigned char seg1[4]={0x08,0x04,0x02,0x01}; 

unsigned int  j = 0, start = 0, TimeTicker = 0;  
unsigned int  ENcnt = 0, temp1 = 0, temp2 = 0; 
long TimeVal = 0; 

void init_input_capture_func() {    // Input Capture function for T/C3
    
    TCCR1A = 0x40; //  CTC Mode PWM (OC1A의 출력 형태 설정)
                    //  COM1A1 = 1, COM1A0 = 0;         
    TCCR1B = 0x09; // CTC, WGM12 = 1, CLK/1                                 
    TCNT1 = 0;       // 타이머1 값 리셋
    //OCR1A = 0;        // PWM Duty = 0             
    TCCR3A = 0x00; // OC3A,B,C normal operation (no PWM by T/C3)  
    SREG = 0x80;  
    
    switch(PINE>>7)                                  
    {
        case 0x01:               // if PE7 Pin is high 
            TCCR3B = 0x81;         // ICNC3=1(input capture noise canceler), 
                                   // ICES3=0(Falling edge), CS3=001(clk/1)  
            PORTG = 0x01;            // Light up LED-1
            break;
        case 0x00:               // if PE7 Pin is low
            TCCR3B = 0xC1;           // ICNC3=1(input capture noise canceler), 
                                    // ICES3=1(Rising edge), CS3=001(clk/1)      
            PORTG = 0x02;             // Light up LED-2
            break;
        default:
            #asm("cli")              // Disabling global inturrput
            break;                                            
    }
    TCNT3H = 0;                  // TCNT3 start value
    TCNT3L = 0;
}

interrupt [TIM3_OVF] void TIM3_OVF_isr(void) { //   T/C3 OVF (주기 약 1[s]) 
    TimeTicker++;
    if(TimeTicker>3)
    {
        temp1 = temp2 = ENcnt =0;
    }
}

interrupt [TIM3_CAPT] void TIM3_CAPT_isr(void) {
    if(start==1){            //-------> 2023.05.25 수정 
        if (ENcnt == 0) {
            temp1 = ICR3L + (ICR3H*256);
            TimeTicker = 0;
        }
        else {
            temp2 = ICR3L + (ICR3H*256);
            TimeVal = (((TimeTicker*65536)+temp2)-temp1); //--> 2023.05.25 수정 
            temp1 = temp2;  
            TimeTicker = 0;      //-------> 2023.05.25 수정 
        }
    ENcnt = 1;
    }
}
               
void main(void)
{   
	DDRG = 0x02+0x01; // DDRG0 = 1 , DDRG1 = 1; 	
	DDRB = 0x20;       //  PWM 포트-> R 출력    
	//DDRB |= 0x40;    //  PWM 포트-> G 출력    
	//DDRB |= 0x80;    //  PWM 포트-> B 출력  
	PORTB = 0x00;      //  출력 테스트용
    DDRD = 0xFF;  
    
    DDRA = 0xFF; // DDRC = 0x08; // Rep241109       
                                     
	while(1) // 무한 루프
    {
		if( Sw2on ) {
     		delay_ms(200); /*Chattering현상 제거 -> Debouncing*/                     
	        if(start==0){
                init_input_capture_func();  // initialize input capture function 
                //(activate input capture noise canceler and toggle between falling and rising edge trigger)                   
                ETIMSK = 0x24;              // INT Enable: TICIE3=1(T/C3 Input Cap.), TOIE3=1(T/C3 OVF)   
                start = 1;                  //-------> 2023.05.25 수정
                OCR1A = 500;
                continue;   
            } 

            TCNT3H = 0;                      // TCNT3 start value
            TCNT3L = 0;                      
            TimeTicker = 0;                  // T/C3 OVF 카운트 초기화 
        if( OCR1A < DSTop ) OCR1A += 500; // PWM Duty 증가                       
        //OCR1B=0*OCR1A; OCR1CH=OCR1AH; OCR1CL=OCR1AL;   
		}
		 
		if( Sw3on ) {                            
		    delay_ms(200);  /* Chattering현상 제거 */
		    if( OCR1A > 0 ) OCR1A -= 500; // PWM Duty 감소   
		    //OCR1B=0*OCR1A; OCR1CH=OCR1AH; OCR1CL=OCR1AL;
		}                
        PORTD = (1<<((OCR1A+1)/500)) - 1;    
        
        for(j=0; j<4; j++)	// j=3으로 해보기
        {
            DDRC = seg1[j];              
            TimeVal = OCR1A;  // 디버깅 출력용 
            if(j==0) PORTA = seg0[TimeVal%10];  
            if(j==1) PORTA = seg0[(TimeVal%100)/10]; 
            if(j==2) PORTA = seg0[(TimeVal%1000)/100]; 
            if(j==3) PORTA = seg0[(TimeVal%10000)/1000]; 
            delay_ms(5);              
        }
	} 	
}
