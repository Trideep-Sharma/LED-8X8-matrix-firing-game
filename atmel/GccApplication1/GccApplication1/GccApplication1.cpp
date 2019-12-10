#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#define rs 0
#define rw 1
#define en 2
void lcdenable();
void cmd(char);
void write(char);
void witeenable();
void clear();
void i2c();
void i2cstart();
void i2caddress();
void i2cinit();
void i2cstop();
void i2cdata(char);
void i2cout();
char seq[10]={0xc0,0xf9,0xa4,0xb0,0xa9,0x92,0x82,0xf8,0x80,0x98};
//char code[10]={0x00,0x80,0x7e,0x20,0x3c,0x10,0x7e,0x08,0x00,0x01};
void keyscan();
int power(int l)
{
	int i,k=1;
	for(i=0;i<l;++i)
	{
		k=k*2;
	}
return k;
}
class player
{
private:
	int current_position;
	int score;
	int frame[8];
	int n;
	int bh[100];
	int bv[100];
	int u,t;
public:
	player()
	{
		current_position=4;
		score=0;
		frame[0]=255;
		frame[1]=255;
		frame[2]=255;
		frame[3]=255;
		frame[4]=255;
		frame[5]=255;
		frame[6]=255;
		frame[7]=255;
		n=0;
		u=0;
		t=0;
		for(int i=0;i<100;++i)
		{
			bh[i]=0;
			bv[i]=0;
		}
	}
	int get_score()
		{
			return score;
		}
	int get_current_position()
		{
			return current_position;
		}
	int get_bv(int q)
		{
			return bv[q];
		}
	int get_bh(int w)
		{
			return bh[w];
		}
	int get_frame(int x)
		{
			return frame[x];
		}
	void set_score(int s)
		{
			score=s;
		}
	void set_current_position(int c)
		{
			current_position=c;
		}
	void increment(int y)
		{
			++bv[y];
			if(bv[y]==6)
			{
				++t;
			}
		}
	void update_frame(player p,int s)
		{
			int j;
			frame[0]=255-(power(current_position)+power(current_position-1)+power(current_position+1));
			frame[1]=255-(power(current_position));
			for(j=t;j<n;++j)
			{
				frame[bv[j]+2]=255-(power(bh[j]));
				++u;
				if((bh[j]==p.get_current_position())&&(bv[j]==4))
				{
					score=score+2;				
				} 
				if(((bh[j]==p.get_current_position()-1)||(bh[j]==p.get_current_position()+1))&&(bv[j]==5))
				{
					++score;
				}
				if(u%25==0)
				{
					if (s==2)
					{
						i2cdata(seq[get_score()]);
						PORTC=PORTC&~(4<<0);
						_delay_us(100);
						PORTC=PORTC|(4<<0);	
					}
					else if(s==1){
						i2cdata(seq[get_score()]);
						PORTC=PORTC&~(8<<0);
						_delay_us(100);
						PORTC=PORTC|(8<<0);	
					}
					PORTC=PORTC&~(12<<0);
					increment(j);
					frame[bv[j]+1]=255; 
				}
			}
		}
	void left()
		{
			if(current_position==1)
			{
				current_position=1;
			}
			else{
				--current_position;
			}
		}
	void right()
		{
			if(current_position==6)
			{
				current_position=6;
			}
			else{
				++current_position;
			}
		}
	void shoot()
		{
			bh[n]=current_position;
			bv[n]=0;
			++n;
			if(n==100)
				{n=0;}
		}
}p1,p2;
void update_screen()
{
	int j;
	int rows[8]={1,2,4,8,16,32,64,128};
	for(j=7;j>=0;--j)
	{
		PORTB=rows[j];
		PORTA=p2.get_frame(7-j);
		_delay_us(350);
	}
	for(j=0;j<8;++j)
	{
		PORTB=rows[j];
		PORTA=p1.get_frame(j);
		_delay_us(350);
	}
}

int main(void)
{
	
	DDRD=0xf0;
	DDRC=0x0f;
	DDRA=0xff;
	DDRB=0xff;
	lcdenable();
	i2c();
	i2cstart();
	i2caddress();
	i2cinit();
	i2cstop();
	i2cstart();
	i2caddress();
	i2cout();
	i2cdata(seq[0]);
	PORTC=PORTC&~(12<<0);
	_delay_us(5);
	PORTC=PORTC|(12<<0);
	
	
    while(1)
    {
	
    keyscan();
	p2.update_frame(p1,2);
	p1.update_frame(p2,1);
	update_screen();
	}
}
void lcdenable()
{
	cmd(0x02);
	cmd(0x28);//4bit mode
	cmd(0x80);//first block
	cmd(0x0E);//cursor with display
}
void cmd(char x)
{
	PORTD=x&(0xf0);
	PORTD=PORTD&~(1<<rs);
	PORTD=PORTD&~(1<<rw);
	PORTD=PORTD|(1<<en);
	_delay_ms(1);
	PORTD=PORTD&~(1<<en);
	_delay_ms(5);
	PORTD=(x<<4);
	PORTD=PORTD&~(1<<rs);
	PORTD=PORTD&~(1<<rw);
	PORTD=PORTD|(1<<en);
	_delay_ms(1);
	PORTD=PORTD&~(1<<en);
	_delay_ms(5);
}
void writeenable()
{
	PORTD=PORTD|(1<<rs);
	PORTD=PORTD&~(1<<rw);
	PORTD=PORTD|(1<<en);
	_delay_ms(1);
	PORTD=PORTD&~(1<<en);
	_delay_ms(5);
	
}
void write(char x)
{
	PORTD=x&(0xf0);
	writeenable();
	PORTD=(x<<4);
	writeenable();
}
void clear()
{
	cmd(0x01);
}

void i2cstart()
{
	TWCR=0x00;
	TWSR=0xf8;
	TWCR=TWCR|(1<<TWEN)|(1<<TWEA)|(1<<TWINT)|(1<<TWSTA);
	while(TWCR&(1<<TWINT)==0){
		//bus is busy
	}
	_delay_ms(300);
	/*switch (TWSR)
	{	
	case 0x08:tx("START BIT WRITTEN SUCCESSFULLY!!\r");
	break;
	case 0x10:tx("START BIT NOT ACKNOWLEDGED WRITING AGAIN!!\r");
	break;
	default:tx("ERROR\r");
	    
	}*/
}
void i2caddress()
{
	TWDR=0x40;
	TWCR=TWCR|(1<<TWEN)|(1<<TWEA)|(1<<TWINT);

	while(TWCR&(1<<TWINT)==0){
		//bus is busy
	}
	_delay_ms(300);
	/*switch (TWSR)
	{
		case 0x18:tx("SLAVE ADDRESS TRANSMITTED AND WRITE ENABLED\r");
		break;
		case 0x20:tx("SLAVE ADDRESS NOT ACKNOWLEDGED!!\r");
		break;
		default:tx("ERROR\r");
	}*/
}
void i2cinit()
{
	TWDR=0x03;
	TWCR=TWCR|(1<<TWEN)|(1<<TWINT)|(1<<TWEA);
	
	while(TWCR&(1<<TWINT)==0){
		//bus is busy
	}
	_delay_ms(300);
	TWDR=0x00;
	TWCR=TWCR|(1<<TWEN)|(1<<TWINT)|(1<<TWEA);
	
	while(TWCR&(1<<TWINT)==0){
		//bus is busy
	}
	_delay_ms(300);
	
}
void i2capply(char x)
{
	
	TWDR=x;
	TWCR=TWCR|(1<<TWEN)|(1<<TWINT)|(1<<TWEA);
	
	while(TWCR&(1<<TWINT)==0){
		//bus is busy
	}
	_delay_ms(300);
}


void i2c()
{
	TWBR=8;
}
void i2cstop()
{
	TWCR=TWCR|(1<<TWEN)|(1<<TWINT)|(1<<TWSTO)|(1<<TWEA);
	_delay_ms(500);
	
}

void i2cdata(char x)
{
	TWDR=x;
	TWCR=TWCR|(1<<TWEN)|(1<<TWINT)|(1<<TWEA);
	
	while(TWCR&(1<<TWINT)==0){
		//bus is busy
	}
	//_delay_ms(300);
	
}	
void i2cout()
{
	TWDR=0x01;
	TWCR=TWCR|(1<<TWEN)|(1<<TWINT)|(1<<TWEA);
	
	while(TWCR&(1<<TWINT)==0){
		//bus is busy
	}
	_delay_ms(300);
}
void keyscan()
{
	
	
	PORTD=0xef;
	if ((PIND&(1<<0))==0)
	{
		p2.right();
		while ((PIND&(1<<0))==0)
		{
			//hold it
		}
	}
	
	if ((PIND&(1<<2))==0)
	{
		p1.left();
		while ((PIND&(1<<2))==0)
		{
			//hold it
		}
	}
	if ((PIND&(1<<3))==0)
	{
		//reset
		while ((PIND&(1<<3))==0)
		{
			//hold it
		}
	}
	PORTD=0xdf;
	if ((PIND&(1<<0))==0)
	{
		p2.shoot();
		while ((PIND&(1<<0))==0)
		{
			//hold it
		}
	}
	
	if ((PIND&(1<<2))==0)
	{
		p1.shoot();
		while ((PIND&(1<<2))==0)
		{
			//hold it
		}
	}
	
	PORTD=0xbf;
	if ((PIND&(1<<0))==0)
	{
		p2.left();
		while ((PIND&(1<<0))==0)
		{
			//hold it
		}
	}
	
	if ((PIND&(1<<2))==0)
	{
		p1.right();
		while ((PIND&(1<<2))==0)
		{
			//hold it
		}
	}


}