#include <mega128.h>
#include <delay.h>
#include <interrupt.h>
#include <stdio.h>
#include <string.h>

#define DSTop 8000
#define ICR3 ( (ICR3H << 8)|ICR3L )
#define Kp 4.0
#define Ki 2
#define Kd 0.4
#define MAX_INTEGRAL 48


const unsigned char seg[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x27, 0x7F, 0x6F };
const unsigned int PPR = 16 * 30;
const float dt = 0.01;

unsigned int OVFCount = 0, zeroSpeedCount = 0, zeroSpeedBias;
int old_angle = 0, current_angle = 0, target_angle = 120, real_velocity = 0, gap_angle;             // (-240~239, 480 각), 16 PPR * 30

// PID 제어기 내부 상태 변수
float error, derivative1=0, derivative2=0, derivative3, control_signal
, previous_error = 0.0, integral = 0.0;

void UART1_init(void){
    UBRR1H = 0x00;    //9,600 baud rate
    UBRR1L = 207;
    UCSR1A |= (1<<U2X1);    //2배속
    
    //비동기, 8비트 데이터, 패리티 없음, 1비트 정지 비트 모드
    UCSR1C |= 0x06;
    UCSR1B |= (1<<RXEN1);    //송수신 가능
    UCSR1B |= (1<<TXEN1);
}
void UART1_transmit(char data){
    while( !(UCSR1A & (1 << UDRE1)) );    //송신 가능 대기
    UDR1 = data;    //데이터 전송
}
unsigned char UART1_receive(void){
    while( !(UCSR1A & (1<<RXC1)) );    //수신 대기
    return UDR1;
}
void UART1_print_string(char *str){    //문자열 송신
    int i;
    for(i = 0; str[i]; i++)    //NULL 문자가 나올 때까지
        UART1_transmit(str[i]);    //바이트 단위 출력
}
void UART1_print_float(float f){
    char numString[20] = "0";
    
    sprintf(numString, "%f", f);
    UART1_print_string(numString);
}

void pid_control(int angle, int target)
{
    gap_angle = target - angle;  
    gap_angle = (gap_angle > (PPR/2) ) ? (gap_angle - PPR ) : (gap_angle < -(PPR/2)) ? ( gap_angle + PPR ) : gap_angle;// over 240, use minus angle
    real_velocity = (angle - old_angle) / dt;   //v = degree(480) / sec
    error = gap_angle; 
    integral += error * dt;                                      // 적분 항 계산, dt = 0.01  
    if (integral > MAX_INTEGRAL) integral = MAX_INTEGRAL;
    else if (integral < -MAX_INTEGRAL) integral = -MAX_INTEGRAL;
    
    derivative3 = derivative2;
    derivative2 = derivative1;
    derivative1 = (error - previous_error) / dt;                  // 미분 항 계산
    control_signal = Kp * error + Ki * integral + Kd * ((derivative1 + derivative2 + derivative3) / 3);    // PID 제어 신호 계산
    previous_error = error;                                       // 이전 오차 갱신 
    
    if( (gap_angle > (PPR/20) ) || (gap_angle < -(PPR/20) ) ){
    
        if( !real_velocity ) { 
            if( ++zeroSpeedCount == 5){                      //to overcome Starting Torque, 5*zeroSpeedCount/16 bias 0.01 sec
                zeroSpeedBias = 1;
            }
        }
        else zeroSpeedCount = 0;
            
        if(control_signal > 0){//CCW, 2928 = (366 RPM / 60) * 480 (degree / revolution)
            if(zeroSpeedBias){  
                control_signal += 366.0 / 60 * PPR / 16 * zeroSpeedCount;
            } 
            OCR1A = (long int)(control_signal  * DSTop / 2928.0);
            OCR1B = 0;
        }
        else {//CW  
            if(zeroSpeedBias){
                control_signal -= 366.0 / 60 * PPR / 16 * zeroSpeedCount;
            }                             
            OCR1A = DSTop - (long int)(-control_signal  * DSTop / 2928.0);
            OCR1B = DSTop;
        }
    }
    else {
        OCR1A = 0;
        OCR1B = 0;  
    }
    //velocity (degree(1920) / sec
    //366RPM = 6.1 / second at 12V 100%;    
}

ISR(EXT_INT7)//encoder pulse, rising edge sense, 1 체배
{   
    if(PINE & 0x40){ 
        if(++current_angle == (PPR/2) ){
            current_angle -= PPR;        //16 PPR * 30( 30:1 ), 480
        }
    }
    else {
        if(--current_angle == -(PPR/2 + 1) ){
            current_angle += PPR;
        }
    }
}

ISR(TIM3_OVF)//every 1ms event
{
    if(++OVFCount == 10)//every 10ms event
    {   
        pid_control(current_angle, target_angle);
        old_angle = current_angle;               
        OVFCount = 0;
    }
}

void main(void)
{                     
    DDRA |= 0xFF;           //7-segment output
    DDRG |= 0x03;           //PORTG 1:0, Orange LED
    DDRB |= 0xE0;           //PORTB 7:5 출력, Motor PWM
    DDRD |= 0xFF;           //8-LED 출력
    DDRC |= (1<<3);         //FND 1의 자리
                                  
    //use TIMER1 to motor duty and rotate direction
    TCCR1A |= 0xA0;         //COMA,COMB Clear on compare match
    TCCR1B |= 0x11;         //Phase and Frequency Correct PWM, Clock Select clk/1 prescaler
    ICR1 = DSTop;           //set TCNT1 top
    TCNT1 = 0;
    OCR1A = 0;
    OCR1B = 0;             //set 0, set DSTop counter wise    
    
    //use TIMER3 to PID calculate timer
    TCCR3A |= 0x01;            //PWM, Phase and Frequency Correct PWM(OCR3A top)
    TCCR3B |= 0x11;            //16000000 Hz
    TCCR3C |= 0;        
    OCR3AH = (DSTop>>8);       //set TCNT3 top
    OCR3AL = DSTop & 0xFF;
    
    //UART1_init(); 
    EICRB = 0xC0;              //Ext_int7 rising edge, int enable      
    EIMSK = 0x80;
    TIMSK |= 0;
    ETIMSK |= 0x04;           
    SREG |= 0x80;
         
    while(1)
    {   //pololu 12V 30:1 motor, 12V 100% duty 366RPM, 6.1 revolution per sec
        //use button to change target_angle
        if(PING & 0x04){
            delay_ms(200);
            target_angle += PPR/8;
            if(target_angle > (PPR/2 - 1) ) target_angle -= PPR;
            PORTG = 0x01;
        }
        if(PING & 0x08){
            delay_ms(200);
            target_angle -= PPR/8;
            if( target_angle < -(PPR/2) ) target_angle += PPR; 
            PORTG = 0x02;
        }
                

        DDRC = (1<<3); 
        PORTA = seg[( (gap_angle<0)?-gap_angle:gap_angle )%10];
        delay_ms(2);
        DDRC = (1<<2);
        PORTA = seg[ (( (gap_angle<0)?-gap_angle:gap_angle )%100)/10];
        delay_ms(2);                                
        DDRC = (1<<1);
        PORTA = seg[ (( (gap_angle<0)?-gap_angle:gap_angle )%1000)/100];
        delay_ms(2);                          
        DDRC = 1;
        PORTA = seg[ (( (gap_angle<0)?-gap_angle:gap_angle )%10000)/1000];
        delay_ms(2);                
        
        //UART1_print_string("motor_velocity: ");
    }
    //추가 필요: 도달시 정지(감속) = 부하 측정? && PID 제어값 보정
}