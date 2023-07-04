/*
 * File:   Calculator.c
 * Author: User
 *
 * Created on 4 July, 2023, 2:20 PM
 */


// PIC16F877A Configuration Bit Settings

// 'C' source line config statements

// CONFIG
#pragma config FOSC = EXTRC     // Oscillator Selection bits (RC oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <math.h>
#define _XTAL_FREQ 6000000
#define X_1    RB0
#define X_2    RB1
#define X_3    RB2
#define X_4    RB3
#define Y_1    RB4
#define Y_2    RB5
#define Y_3    RB6
#define Y_4    RB7


void keypad_init(void);
unsigned char key_press_scan(void);
unsigned char keypad_scanner(void);
int select_val(unsigned char v);
int keypad_calc(unsigned char i[], unsigned char j, unsigned char k[]);
unsigned char val, k[10], n, m, a1[5], a2, a3[5], v1, v2, cnt1=1, cnt2=1;

void lcd_init(void);
void lcd_cmd(unsigned char i);
void lcd_data(unsigned char i);
void lcd_out(unsigned int i);
unsigned char msg[20] = "CALCULATOR";
unsigned char err[20] = "MATH ERROR";

void main(){
    a1[0]='\0';
    a3[0]='\0';
    lcd_init();
    keypad_init();
    unsigned char key = 'n';
    lcd_cmd(0x80);
    for (int i=0; i<10; i++){
        n=msg[i];
        lcd_data(n);
    }
    __delay_ms(100);
    lcd_cmd(0xC0);
    lcd_cmd(0x01); //0000 0001 : Clear Display
    __delay_ms(100);
    while(1){
        key=key_press_scan();
        if (key=='C'){
            lcd_cmd(0x01); //0000 0001 : Clear Display
            __delay_ms(100);
            m=0;
        }
        else if (key=='+' || key=='-' || key=='x' || key=='/'){
            if (cnt1<5){
                for (int j=cnt1; j<5; j++){
                    a1[j]='#';
                }
            }
            lcd_data(key);
            a2=key;
            m=1;
            cnt1=1;
        }
        else if (key=='='){
            lcd_data(key);
            if (cnt2<5){
                for (int j=cnt2; j<5; j++){
                    a3[j]='#';
                }
            }
            int ans=keypad_calc(a1,a2,a3);
            if (ans<0){
                ans = -1*(ans);
                lcd_data('-');
                lcd_out(ans);
            }
            else{
                lcd_out(ans);
            }
            cnt2=1;
            m=0;
        }
        else{
            if (m==0){
                a1[cnt1]=key;
                lcd_data(key);
                m=0;
                cnt1+=1;
            }
            else if (m==1){
                a3[cnt2]=key;
                lcd_data(key);
                m=1;
                cnt2+=1;
            }
            else{
                for (int i=0; i<10; i++){
                    n=err[i];
                    lcd_data(n);
                }
                lcd_cmd(0x01); //0000 0001 : Clear Display
                __delay_ms(100);
            }
        }
    }
}

void keypad_init(void)
{
    PORTB=0x00; 
    TRISB=0xF0; //Keypad is connected to PORTB
    //RB0 to RB3 => Row1 to Row4
    //RB4 to RB6 => Col1 to Col3
    OPTION_REG &= 0x7F;
}

unsigned char key_press_scan(void) //Get key from user
{
   unsigned char key = 'n'; //Assume no key pressed
   while(key=='n') //Wait until a key is pressed
       key=keypad_scanner(); //scan the keys again and again
   return key;
}

unsigned char keypad_scanner(void)
{
    //Keeping row1=0 and reading the status of each column
    X_1 = 0; X_2 = 1; X_3 = 1; X_4 = 1;    
            if (Y_1 == 0) { __delay_ms(100); while (Y_1==0); return '7'; }
            if (Y_2 == 0) { __delay_ms(100); while (Y_2==0); return '8'; }
            if (Y_3 == 0) { __delay_ms(100); while (Y_3==0); return '9'; }
            if (Y_4 == 0) { __delay_ms(100); while (Y_4==0); return '/'; }

    //Keeping row2=0 and reading the status of each column    
    X_1 = 1; X_2 = 0; X_3 = 1; X_4 = 1;    
            if (Y_1 == 0) { __delay_ms(100); while (Y_1==0); return '4'; }
            if (Y_2 == 0) { __delay_ms(100); while (Y_2==0); return '5'; }
            if (Y_3 == 0) { __delay_ms(100); while (Y_3==0); return '6'; }
            if (Y_4 == 0) { __delay_ms(100); while (Y_4==0); return 'x'; }

    //Keeping row3=0 and reading the status of each column  
    X_1 = 1; X_2 = 1; X_3 = 0; X_4 = 1;    
            if (Y_1 == 0) { __delay_ms(100); while (Y_1==0); return '1'; }
            if (Y_2 == 0) { __delay_ms(100); while (Y_2==0); return '2'; }
            if (Y_3 == 0) { __delay_ms(100); while (Y_3==0); return '3'; }
            if (Y_4 == 0) { __delay_ms(100); while (Y_4==0); return '-'; }

    //keeping row4=0 and reading the status of each column      
            X_1 = 1; X_2 = 1; X_3 = 1; X_4 = 0;    
            if (Y_1 == 0) { __delay_ms(100); while (Y_1==0); return 'C'; }
            if (Y_2 == 0) { __delay_ms(100); while (Y_2==0); return '0'; }
            if (Y_3 == 0) { __delay_ms(100); while (Y_3==0); return '='; }
            if (Y_4 == 0) { __delay_ms(100); while (Y_4==0); return '+'; }
            
    return 'n';
}

int select_val(unsigned char v){
    switch (v){
        case '1':
            return 1;
        case '2':
            return 2;
        case '3':
            return 3;
        case '4':
            return 4;
        case '5':
            return 5;
        case '6':
            return 6;
        case '7':
            return 7;
        case '8':
            return 8;
        case '9':
            return 9;
        case '0':
            return 0;
    }
}


int keypad_calc(unsigned char i[], unsigned char j, unsigned char k[]){
    int res, count=0;
    int v1, v2, temp[3];
    temp[0]=0;
    temp[1]=0;
    temp[2]=0;
    count=0;
    while (i[count+1]!='#'){
		count+=1;
	}
    if (count==3){
        int x1=1, x2=10, x3=100;
        temp[0]=select_val(i[1])*x3;
        temp[1]=select_val(i[2])*x2;
        temp[2]=select_val(i[3])*x1;
    }
    else if (count==2){
        int x1=0, x2=1, x3=10;
        temp[0]=select_val(i[1])*x3;
        temp[1]=select_val(i[2])*x2;
        temp[2]=select_val(i[3])*x1;
    }
    else if (count==1){
        int x1=0, x2=0, x3=1;
        temp[0]=select_val(i[1])*x3;
        temp[1]=select_val(i[2])*x2;
        temp[2]=select_val(i[3])*x1;
    }
    v1=(int)(temp[0]+temp[1]+temp[2]);
    count=0;
    while (k[count+1]!='#'){
		count+=1;
	}
    if (count==3){
        int x1=1, x2=10, x3=100;
        temp[0]=select_val(k[1])*x3;
        temp[1]=select_val(k[2])*x2;
        temp[2]=select_val(k[3])*x1;
    }
    else if (count==2){
        int x1=0, x2=1, x3=10;
        temp[0]=select_val(k[1])*x3;
        temp[1]=select_val(k[2])*x2;
        temp[2]=select_val(k[3])*x1;
    }
    else if (count==1){
        int x1=0, x2=0, x3=1;
        temp[0]=select_val(k[1])*x3;
        temp[1]=select_val(k[2])*x2;
        temp[2]=select_val(k[3])*x1;
    }
    v2=(int) (temp[0]+temp[1]+temp[2]);
    switch (j){
        case '+':
            res = v1+v2;
            break;
        case '-':
            res = v1-v2;
            break;
        case 'x':
            res = v1*v2;
            break;
        case '/':
            res = v1/v2;
            break;
        default:
            lcd_cmd(0x01); //0000 0001 : Clear Display
            __delay_ms(100);     
    }
    return res;
}
void lcd_init(void)
{
    TRISC=0x00;
    TRISD=0x00;
    lcd_cmd(0x38);
    __delay_ms(100);
    lcd_cmd(0x38);
    __delay_ms(100);
    lcd_cmd(0x38);
    __delay_ms(100);
    lcd_cmd(0x38);
    __delay_ms(100);
    lcd_cmd(0x08);
    __delay_ms(100);
    lcd_cmd(0x0C);
    __delay_ms(100);
    lcd_cmd(0x01);
    __delay_ms(100);
    lcd_cmd(0x06);
    __delay_ms(100);
}

void lcd_cmd(unsigned char i)
{
    PORTC &=~ 0x08; //RC3 = 0  RC3 is connected to RS=0(COMMAND)
    PORTD = i;  //values of bits in i are assigned to corresponding Pins in PORT D 
    PORTC |= 0x01; //Enable = HIGH (RC0 = 1)
    PORTC &=~ 0x01; //Enable = LOW (RC0 = 0)
    __delay_ms(100);
}

void lcd_data(unsigned char i)
{
    PORTC |= 0x08; //RC3 = 1  RC3 is connected to RS=1(DATA)
    PORTD = i; //values of bits in i are assigned to corresponding Pins in PORT D
    PORTC |= 0x01; //Enable = HIGH (RC0 = 1)
    PORTC &=~ 0x01; //Enable = LOW (RC0 = 0)
    __delay_ms(100);
}

void lcd_out(unsigned int i)
{
    unsigned char s,j=1;
    m=i;
    while(m!=0)
    {
        s=m-((m/10)*10); //last digit of m is stored in s
        k[j]=s; //s is assigned to jth index value of k
        j++; //Increment j by 1
        m=m/10; //Divide m by 10 and store the quotiont in m 
    }
    k[j]='\0'; //Null Character
    j=j-1; //decrement j by 1
    while(j!=0)
    {
        n=0x30+k[j]; //"0" + k[j] = actual data
        lcd_data(n); //displaying data
        j--; //Decrement j by 1
    }
}