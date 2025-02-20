#include <mega128.h>
#include <delay.h>
#include <stdio.h>
#include <string.h>
#include <interrupt.h>

#define DSTop 8000
#define ICR3 ((ICR3H << 8)|ICR3L)
#define MAX_INTEGRAL 48

const unsigned char seg[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x27, 0x7F, 0x6F};
const unsigned int PPR = 16 * 30;
const float dt = 0.01;

unsigned char ControlMode = 0, LastControlMode = 1, PIDEnable = 1;          //ControlMode: 0=gap 1=Kp 2=Ki 3=Kd 
unsigned int OVFCount = 0, zeroSpeedCount = 0, zeroSpeedBias, ADCLast;
int old_angle = 0, current_angle = 0, target_angle = 120, real_velocity = 0, gap_angle, fndValue;             // (-240~239, 480 각), 16 PPR * 30

// PID 제어기 내부 상태 변수
float error, derivative1=0, derivative2=0, derivative3, control_signal
, previous_error = 0.0, integral = 0.0, Kp = 5.0, Ki = 0.0, Kd = 0.0;

int mapping(int x, int in_MIN, int in_MAX, int out_MIN, int out_MAX){
    return (x-in_MIN) * (out_MAX - out_MIN)/(in_MAX - in_MIN) + out_MIN;
}
void UART0_init(void){
    UBRR0H = 0x00;    //9,600 baud rate
    UBRR0L = 16.36;
    UCSR0A |= 1 << U2X0;    //2배속
    //비동기, 8비트 데이터, 패리티 없음, 1비트 정지 비트 모드
    UCSR0C |= 0x06;
    UCSR0B |= 1 << RXEN0;    //송수신 가능
    UCSR0B |= 1 << TXEN0;
}
void UART0_transmit(char data){
    while( !(UCSR0A & (1 << UDRE0)) );    //송신 가능 대기
    UDR0 = data;    //데이터 전송
}
unsigned char UART0_receive(void){
    while( !(UCSR0A & (1<<RXC0)) );    //수신 대기
    return UDR0;
}
void UART0_print_string(char *str){
    int i;
    for(i = 0; str[i]; i++)
        UART0_transmit(str[i]);
}
void UART0_print_float(float f){
    char numString[20] = "0";
    
    sprintf(numString, "%f", f);
    UART0_print_string(numString);
}

void displayFND(int value3, int value2, int value1, int value0, char point)
{
    int i;
    for(i = 0; i<4; i++)  
    {
        DDRC = (1<< (3-i) );  
        switch(i)
        { 
            case 0:
                PORTA = value0;
                break; 
            case 1:
                PORTA = value1;
                break;  
            case 2:
                PORTA = value2;      
                break;
            case 3:
                PORTA = value3;
                break;
            default:
                break;
        }
        if(point & (1 << i)) PORTA |= 0x80;
        delay_ms(2);
    }
}

void pid_control(void)
{
    real_velocity = (current_angle - old_angle) / dt;   //v = degree(480) / sec
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
    //velocity (degree(480) / sec
    //366RPM = 6.1 / second at 12V 100%;    
}

ISR(EXT_INT0)
{
    ;


}

ISR(EXT_INT1)     //encoder +
{   
    if((PIND & 0x03) == 3) target_angle--;
    else if((PIND & 0x03) == 1) target_angle++;
    else if( (PIND & 0x03) == 0) target_angle--;
    else if( (PIND & 0x03) == 2) target_angle++;
}

ISR(EXT_INT2)     //encoder switch
{
    if( !ControlMode ) ControlMode = LastControlMode;
    else 
    {
        ++ControlMode; 
        ControlMode %= 4;
    }
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

ISR(TIM3_OVF)//every 1ms event M method
{
    if(++OVFCount == 10)//every 10ms event
    {           
        gap_angle = target_angle - current_angle;  
        gap_angle = (gap_angle > (PPR/2) ) ? (gap_angle - PPR ) : (gap_angle < -(PPR/2)) ? ( gap_angle + PPR ) : gap_angle;// over 240, use minus angle                         
        if(PIDEnable) pid_control();
        old_angle = current_angle;               
        OVFCount = 0;
    }
}

ISR(ADC_INT) //use voltage as target_angle
{
    target_angle = mapping( ((int)ADCL|((int)ADCH<<8)), 0, 1023, -240, 239); 
    if(ADCLast != target_angle)
    { 
        LastControlMode = ControlMode;
        ControlMode = 0;
    }
    ADCLast = target_angle;
}

void main(void)
{                     
    DDRA |= 0xFF;           //7-segment output     
    DDRB |= 0xE0;           //PORTB 7:5 출력, Motor PWM
    DDRC |= (1<<3);         //FND 1의 자리         
    DDRD |= 0x00;           //Encoder 입력 (EXT_INT0, EXT_INT1)           
    DDRF |= 0x00;           //ADC input
    DDRG |= 0x03;           //PORTG 1:0, Orange LED
                                  
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
                                       
    UART0_init();
    ADMUX |= 0x47;             //ADC7 enable
    ADCSRA |=0xEF;             //ADC enable, ADC start, ADC free running mode, ADC Interrupt enable  
    EICRA = 0x3F;              //EXT_INT 0 1 2 rising dege, int enable
    EICRB = 0xC0;              //Ext_int7 rising edge, int enable      
    EIMSK = 0x87;
    TIMSK |= 0;
    ETIMSK |= 0x04;
    ADMUX |= 0x47;             //ADC7 channel 
    ADCSRA |=0xEF;             // free running mode, IE, prescaler 128
    SREG |= 0x80;
         
    while(1)
    {   //pololu 12V 30:1 motor, 12V 100% duty 366RPM, 6.1 revolution per sec
        //use button to change target_angle
        if(PING & 0x04){ 
            PIDEnable = 1;
            PORTG = 0x01;
        }
        if(PING & 0x08){
            PIDEnable = 0;    
            Ki = 0;    
            derivative1 = 0;
            derivative2 = 0;
            derivative3 = 0;
            PORTG = 0x02;
        }          
        switch(ControlMode)    //ControlMode: 0=gap 1=Kp 2=Ki 3=Kd
        {
            case 0:   
                fndValue = gap_angle;    
                displayFND(
                            seg[(fndValue%10000)/1000],
                            seg[(fndValue%1000)/100],
                            seg[(fndValue%100)/10],
                            seg[fndValue%10],
                            0);  
                break;
            case 1:    
                fndValue = (int)(Kp*10);                    
                displayFND(
                            0x73,
                            seg[(fndValue%1000)/100],
                            seg[(fndValue%100)/10],
                            seg[fndValue%10],
                            2);
                break;
            case 2:                  
                fndValue = (int)(Ki*10); 
                displayFND(
                            0x30,
                            seg[(fndValue%1000)/100],
                            seg[(fndValue%100)/10],
                            seg[fndValue%10],
                            2);  
                break; 
            case 3:
                fndValue = (int)(Kd*10);                       
                displayFND(
                            0x5E,
                            seg[(fndValue%1000)/100],
                            seg[(fndValue%100)/10],
                            seg[fndValue%10],
                            2); 
                break;
            default:
                fndValue = gap_angle;                      
                displayFND(
                            seg[(fndValue%10000)/1000],
                            seg[(fndValue%1000)/100],
                            seg[(fndValue%100)/10],
                            seg[fndValue%10],
                            0);
                break;
        }            
        
    }
}