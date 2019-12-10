#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
void i2c();
void i2cstart();
void i2caddress();
void i2cinit();
void i2cstop();
void i2cdata(char,char);
void i2cout();
void spi(char);
char seq[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0xff,0x67};
void keyscan();
void reset();
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
	int get_frame(int x)
		{
			return frame[x];
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
				
				if(u%25==0)
				{
					if((bh[j]==p.get_current_position())&&(bv[j]==4))
					{
						score=score+2;
						if (s==1)
						{
							i2cdata(seq[get_score()],seq[p.get_score()]);
														
						}
						else if(s==2){
								i2cdata(seq[p.get_score()],seq[get_score()]);
							
						}
						
						if (score==9)
						{
							
							while((PIND&(1<<0))==0)
							{	if(s==2)
									{
										i2cdata(seq[p.get_score()],seq[get_score()]);
										_delay_ms(2000);
										i2cdata(seq[p.get_score()],0x00);
										_delay_ms(2000);
									}		
								else if(s==1)
									{
										i2cdata(seq[p.get_score()],seq[get_score()]);
										_delay_ms(2000);
										i2cdata(0x00,seq[get_score()]);
										_delay_ms(2000);
									}							
							}
							reset();
							
						}
						
					}
					if(((bh[j]==p.get_current_position()-1)||(bh[j]==p.get_current_position()+1))&&(bv[j]==5))
					{
						++score;
						if (s==1)
						{
							
							i2cdata(seq[get_score()],seq[p.get_score()]);
							
						}
						else if(s==2){
												
							i2cdata(seq[p.get_score()],seq[get_score()]);
						if (score==9)
						{
							
							while((PIND&(1<<0))==0)
							{	if(s==2)
								{
									i2cdata(seq[p.get_score()],seq[get_score()]);
									_delay_ms(2000);
									i2cdata(seq[p.get_score()],0x00);
									_delay_ms(2000);
								}
								else if(s==1)
								{
									i2cdata(seq[p.get_score()],seq[get_score()]);
									_delay_ms(2000);
									i2cdata(0x00,seq[get_score()]);
									_delay_ms(2000);
								}
							}
							reset();
							
						}
							
						}
						
					}
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
		PORTB=(rows[j]&0x0f);
		PORTD=(rows[j]&0xf0);
		PORTA=p2.get_frame(7-j);
		_delay_us(350);
	}	
	for(j=0;j<8;++j)
	{
		PORTB=(rows[j]&0x0f);
		PORTD=(rows[j]&0xf0);
		PORTA=p1.get_frame(j);
		_delay_us(350);
	}
}

int main(void)
{
	
	DDRD=0xf0;
	DDRC=DDRC|(3<<0);
	DDRA=0xff;
	DDRB=0xbf;
	i2c();
	i2cstart();
	i2caddress();
	i2cinit();
	i2cstop();
	i2cstart();
	i2caddress();
	i2cout();
	SPCR=0b01110000;
	PORTB=PORTB&~(1<<5);
	spi(0x40);
	spi(0x05);
	spi(0x0b00111100);
	PORTB=PORTB|(1<<5);
	_delay_us(500);
	PORTB=PORTB&~(1<<5);
	spi(0x40);
	spi(0x00);
	spi(0x00);
	PORTB=PORTB|(1<<5);
	_delay_us(500);
	PORTB=PORTB&~(1<<5);
	spi(0x40);
	spi(0x01);
	spi(0x00);
	PORTB=PORTB|(1<<5);
	_delay_us(500);
	PORTB=PORTB&~(1<<5);
	spi(0x40);
	spi(0x12);
	spi(0xff);
	PORTB=PORTB|(1<<5);
	_delay_us(500);
	PORTB=PORTB&~(1<<5);
	spi(0x40);
	spi(0x13);
	spi(0xff);
	PORTB=PORTB|(1<<5);
	i2cdata(seq[0],seq[0]);
	_delay_us(1000);
	
    while(1)
    {
	if(PIND&(1<<0))
		reset();
    keyscan();
	p2.update_frame(p1,2);
	p1.update_frame(p2,1);
	update_screen();
	}
}
void spi(char z)
{
	SPDR=z;
	while((SPSR&(1<<SPIF))==0){
		//writing
	}
	SPSR=SPSR|(1<<SPIF);
}
void reset()
{
	p1=player();
	p2=player();
	//i2cdata(seq[p1.get_score()],seq[p2.get_score()]);
}
void i2cstart()
{
	TWCR=0x00;
	TWSR=0xf8;
	TWCR=TWCR|(1<<TWEN)|(1<<TWEA)|(1<<TWINT)|(1<<TWSTA);
	while(TWCR&(1<<TWINT)==0){
		//bus is busy
	}
	_delay_us(500);
}
void i2caddress()
{
	TWDR=0x40;
	TWCR=TWCR|(1<<TWEN)|(1<<TWEA)|(1<<TWINT);

	while(TWCR&(1<<TWINT)==0){
		//bus is busy
	}
	_delay_us(500);
}
void i2cinit()
{
	TWDR=0x06;
	TWCR=TWCR|(1<<TWEN)|(1<<TWINT)|(1<<TWEA);
	
	while(TWCR&(1<<TWINT)==0){
		//bus is busy
	}
	_delay_ms(100);
	TWDR=0x00;
	TWCR=TWCR|(1<<TWEN)|(1<<TWINT)|(1<<TWEA);
	
	while(TWCR&(1<<TWINT)==0){
		//bus is busy
	}
	_delay_ms(100);
	TWDR=0x00;
	TWCR=TWCR|(1<<TWEN)|(1<<TWINT)|(1<<TWEA);
	
	while(TWCR&(1<<TWINT)==0){
		//bus is busy
	}
	
	_delay_us(500);
}
void i2capply(char x)
{
	
	TWDR=x;
	TWCR=TWCR|(1<<TWEN)|(1<<TWINT)|(1<<TWEA);
	
	while(TWCR&(1<<TWINT)==0){
		//bus is busy
	}
	_delay_us(500);
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

void i2cdata(char x,char y)
{
	TWDR=x;
	TWCR=TWCR|(1<<TWEN)|(1<<TWINT)|(1<<TWEA);
	
	while(TWCR&(1<<TWINT)==0){
		//bus is busy
	}
	_delay_ms(10);
	TWDR=y;
	TWCR=TWCR|(1<<TWEN)|(1<<TWINT)|(1<<TWEA);
	
	while(TWCR&(1<<TWINT)==0){
		//bus is busy
	}
	
	_delay_us(500);
}	
void i2cout()
{
	TWDR=0x02;
	TWCR=TWCR|(1<<TWEN)|(1<<TWINT)|(1<<TWEA);
	
	while(TWCR&(1<<TWINT)==0){
		//bus is busy
	}
	_delay_us(500);
}
void keyscan()
{
	
	if ((PINC&(1<<2)))
	{
		p2.right();
		while ((PINC&(1<<2)))
		{
			//hold it
		}
	}
	
	if ((PINC&(1<<5)))
	{
		p1.left();
		while ((PINC&(1<<5)))
		{
			//hold it
		}
	}

	if ((PINC&(1<<3)))
	{
		p2.shoot();
		while ((PINC&(1<<3)))
		{
			//hold it
		}
	}
	
	if ((PINC&(1<<6)))
	{
		p1.shoot();
		while ((PINC&(1<<6)))
		{
			//hold it
		}
	}
	
	if ((PINC&(1<<4)))
	{
		p2.left();
		while ((PINC&(1<<4)))
		{
			//hold it
		}
	}
	
	if ((PINC&(1<<7)))
	{
		p1.right();
		while ((PINC&(1<<7)))
		{
			//hold it
		}
	}


}



