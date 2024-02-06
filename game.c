#include <msp430x14x.h>
#include "lcd.h"
#include "portyLcd.h"
#include "string.h"

unsigned char znak;
unsigned char punkt='$';
unsigned char kamien='#';
unsigned int zycie;
int wynik;
unsigned char nick[11]="           ";
unsigned char nicki[3][11];
int wyn[3]= {-1,-1,-1};
unsigned char w1[16], w2[16];
void wyswietl1(char*, char*);
void wyswietl2(char*, char*);
void InitSpecial();
void losuj();
void Wstep();
void Menu();
void Buzzer();
void wyswietl_wynik();
void gra();
void opis();
void autorzy();
void edycja_highscore();
void podaj_nick();
void wyniki();
void wyswietlnick();


void main( void )
{

    WDTCTL=WDTPW+ WDTHOLD; // zatrzymanie WDT
    P2DIR |= BIT1 ; // STATUS LED
    InitPortsLcd(); // inicjalizacja port?w
    InitLCD(); // inicjalizacja LCD
    clearDisplay(); // czyszczenie LCD
    P4DIR = 0x0F;
    InitSpecial();

    srand(time(NULL));

    Wstep();
    for(long int i=0; i<100000; i++);
    Menu();
}

void InitSpecial()
{
    SEND_CMD(CG_RAM_ADDR); //wejscie do pamieci wyswietlacza
    int spec[2][8]=
    {
        {0x0D,0x0A,0x0D,0x04,0x1E,0x04,0x04,0x0A}, //ludzik
        {0x1E,0x15,0x1E,0x1B,0x1E,0x09,0x12,0x09}
    }; //kamien
    for (int i=0; i<2; i++)
    {
        for (int j=0; j<8; j++)
        {
            SEND_CHAR(spec[i][j]);
            //wys?anie zdefiniowanych znak?w do pami?ci wy?wietlacza
        }
    }
}


void losuj()
{

    int rodzaj,linia;
    rodzaj=rand()%2+1;
    //wtedy kamien
    if(rodzaj==1)
    {
        linia=rand()%2+1;
        if(linia==1)
        {
            w1[15]=kamien;
        }
        else w2[15]=kamien;
    }
    //wtedy punkt
    else
    {
        linia=rand()%2+1;
        if(linia==1)
        {
            w1[15]=punkt;
        }
        else w2[15]=punkt;
    }
}
void wyswietlnick(char w1[])
{
    clearDisplay();
    char start[16]="Podaj nazwe     ";
    SEND_CMD(DD_RAM_ADDR);
    for(int i=0; i<16; i++)
    {
        SEND_CHAR(start[i]);
    }
    SEND_CMD(DD_RAM_ADDR2);
    for(int i=0; i<11; i++)
    {
        SEND_CHAR(w1[i]);
    }

}

void wyswietl1(char w1[], char w2[])
{

    SEND_CMD(DD_RAM_ADDR);
    for(int i=0; i<16; i++)
    {
        if(w1[i] == '%')
            SEND_CHAR(0);
        else if(w1[i] == '#')
            SEND_CHAR(1);
        else
            SEND_CHAR(w1[i]);
    }
    SEND_CMD(DD_RAM_ADDR2);
    for(int i=0; i<16; i++)
    {
        if(w2[i] == '%')
            SEND_CHAR(0);
        else if(w2[i] == '#')
            SEND_CHAR(1);
        else
            SEND_CHAR(w2[i]);
    }
}
void wyswietl2(char w1[], char w2[])
{
    SEND_CMD(DD_RAM_ADDR2);
    for(int i=0; i<16; i++)
    {
        if(w2[i] == '%')
            SEND_CHAR(0);
        else if(w2[i] == '#')
            SEND_CHAR(1);
        else
            SEND_CHAR(w2[i]);
    }
    SEND_CMD(DD_RAM_ADDR);
    for(int i=0; i<16; i++)
    {
        if(w1[i] == '%')
            SEND_CHAR(0);
        else if(w1[i] == '#')
            SEND_CHAR(1);
        else
            SEND_CHAR(w1[i]);
    }
}
void Wstep()
{
    P2OUT|=BIT1;
    clearDisplay();
    char tab1[16]="  Witamy w grze ";
    char tab2[16]=" Zdobywca skarbu";
    wyswietl1(tab1,tab2);
}
void Menu()
{
    clearDisplay();
    char tab1[16]="1.Opis 2.Autorzy";
    char tab2[16]="3.Wyniki 4.Graj ";
    wyswietl1(tab1,tab2);
    for(;;)
    {
        if(!(P4IN&BIT4))
        {
            opis();
            Menu();

        }
        else if(!(P4IN&BIT5))
        {
            autorzy();
            Menu();

        }
        else if(!(P4IN&BIT6))
        {
            wyniki();
            Menu();

        }
        else if(!(P4IN&BIT7))
        {
            gra();
            Menu();
        }

    }
}

void Buzzer(void)
{
    int i;
    P4OUT|=0x08; //stan wysoki na wyjociu P4.4
    P4OUT&=~0x04; //stan niski na wyjociu P4.3
    for(i=0; i<200; i++)
    {} //opoznienie
    P4OUT|=0x04; // stan wysoki na wyjsciu P4.3
    P4OUT&=~0x08; //stan niski na wyjsciu P4.4
    for(i=0; i<200; i++) {} //opoznienie
}
void wyswietl_wynik()
{
    int wyn1=wynik;
    clearDisplay();
    char tab1[16]="Wynik:     ";
    char tab[16]; //deklaracja 16 elementowej tablicy char
    int i, dlugosc=0;
    SEND_CMD(DD_RAM_ADDR);
    for(i=0; i<strlen(tab1); i++)
    {
        SEND_CHAR(tab1[i]);
    }
    for (i=0;; i++)
    {
        tab[i]=(char)(wyn1%10);
        wyn1/=10;

        dlugosc+=1;
        if(wyn1==0)break;
    }
    SEND_CMD(DD_RAM_ADDR2); //ustawienie sie w pierwszej linii wyswietlacza
    for (int i=dlugosc-1; i>=0; i--)
    {
        SEND_CHAR(tab[i]+48);
        //wyswietlenie cyfry danej liczby +48, poniewa? 0 ma w kod 48 w tablicy kodów ASCII
    }
}

void gra()
{
    for(int i=0; i<16; i++)
    {
        w1[i]=' ';
        w2[i]=' ';
    }
    znak='%';
    w1[2]=znak;

    wyswietl1(w1, w2);
    int n=0;
    zycie=2;
    P2OUT&=~BIT1;
    while (1) // niesko?czona p?tla
    {
        if(zycie==0)
        {
            P2OUT|=BIT1;
            break;
        }
        if(zycie==1)
            P2OUT^=BIT1;

        if((!(P4IN&BIT4))&& w2[2]==znak)
        {
            w1[2]=znak;
            w2[2]=' ';
            wyswietl2(w1, w2);
        }
        if((!(P4IN&BIT5))&& w1[2]==znak)
        {
            w2[2]=znak;
            w1[2]=' ';
            wyswietl1(w1, w2);
        }
        for(int i=0; i<16; i++)
        {
            if(i==15)
            {
                w1[i]=' ';
                w2[i]=' ';
                continue;
            }

            if((w1[i]!=znak)&&(w1[i+1]!=znak))
                w1[i]=w1[i+1];
            else if(w1[i+1]==znak)
                w1[i]=' ';
            else if (w1[i]==znak)
            {
                if(w1[i+1]==kamien)
                {
                    zycie--;
                    for(int i=0; i<100; i++)
                        Buzzer();
                }
                else if(w1[i+1]==punkt)
                {
                    wynik+=1;
                }
            }
            if((w2[i]!=znak)&&(w2[i+1]!=znak))
                w2[i]=w2[i+1];
            else if(w2[i+1]==znak)
                w2[i]=' ';
            else if (w2[i]==znak)
            {
                if(w2[i+1]==kamien)
                {
                    zycie--;
                    for(int i=0; i<100; i++)
                        Buzzer();
                }
                else if(w2[i+1]==punkt)
                {
                    wynik+=1;
                }
            }

        }

        n++;
        if(n%4==0)
        {
            losuj();
        }
        for(int i=0; i<100; i++);
        wyswietl1(w1,w2);
    }
    wyswietl_wynik();
    for(long int i=0; i<100000; i++);
    //podaj_nick();
    edycja_highscore();
    for(long int i=0; i<50000; i++);
    wynik=0;


}
void opis()
{
    clearDisplay();
    char tab[9][16]= {{"Gra polega na   "},{"omijaniu #      "},{"i zbieraniu $   "},{"Poruszasz sie   "}
        ,{"2 przyciskami   "}, {"p1 - w gore     "}, {"p2 - w dol      "},{"Masz dwa zycia  "},{"Powodzenia!!    "}
    };
    for(int i=0; i<8; i++)
    {
        wyswietl1(tab[i],tab[i+1]);
        for(long int j=0; j<100000; j++);

    }
}
void autorzy()
{
    clearDisplay();
    char au[6][16] = {{"Mateusz         "}, {"Sokolowski      "}, {"Mateusz         "},
        {"Strapczuk       "},{"Pawel           "},{"Krol            "}
    };


    for(int i=0; i<6; i+=2)

    {
        wyswietl1(au[i],au[i+1]);
        for(long int j=0; j<100000; j++);
    }
}
void wyniki()
{
    for(int i=0; i<3; i++)

    {
        if(wyn[i]!=-1)
        {
            clearDisplay();
            int wyn1=wyn[i];
            char tab[16];
            int dlugosc=0;
            SEND_CMD(DD_RAM_ADDR);
            for(int j=0; j<11; j++)
            {
                SEND_CHAR(nicki[i][j]);
            }
            for (int j=0;; j++)
            {
                tab[j]=(char)(wyn1%10);
                wyn1/=10;

                dlugosc+=1;
                if(wyn1==0)break;
            }
            SEND_CMD(DD_RAM_ADDR2); //ustawienie sie w pierwszej linii wyswietlacza
            for (int j=dlugosc-1; j>=0; j--)
            {
                SEND_CHAR(tab[j]+48);
                //wyswietlenie cyfry danej liczby +48, poniewa? 0 ma w kod 48 w tablicy kodów ASCII
            }
            for(long int j=0; j<100000; j++);
        }

    }
}
void podaj_nick()
{
    int i=0;
    char litera=65;
    nick[0]=litera;//duze A
    for(;;)
    {
        if(!(P4IN&BIT7))
        {
            break;
        }
        //przejscie pod kolejny indeks tablicy
        if(!(P4IN&BIT6))
        {
            litera=65; //duze A
            i++;
            if(i==11) break;
            nick[i]=litera;
            for( int j=0; j<500; j++) {} //opoznienie
            //funckja wyswietl do wyswietlenia nicku
        }
        if(!(P4IN&BIT5))
        {
            litera++;
            if(litera>90)
            {
                litera=65;   //jesli litera jest wieksza od Z to litera = A
            }
            nick[i]=litera;
            for( int j=0; j<500; j++) {} //opoznienie
            //funkcja wyswietl do wyswietlenia nicku
        }
        if(!(P4IN&BIT4))
        {
            litera--;
            if(litera<65)
            {
                litera=90;   //jesli litera jest mniejsza od A to litera = Z
            }
            nick[i]=litera;
            for( int j=0; j<1000; j++) {} //opoznienie
            //funkcja wyswietl do wyswietlenia nicku
        }
        wyswietlnick(nick);
        for(int j=0; j<3000; j++) {} //opoznienie
    }
}


void edycja_highscore()
{
    //tablica nick to tablica globalna
    if(wynik>wyn[0])
    {
        podaj_nick();
        //tutaj powinna sie wywolywac funkcja podaj nick
        int tmp1,tmp2;
        tmp1=wyn[0];
        tmp2=wyn[1];
        wyn[0]=wynik;
        wyn[1]=tmp1;
        wyn[2]=tmp2;
        char pom1, pom2;
        for(int i=0; i<11; i++)
        {
            pom1=nicki[0][i];
            nicki[0][i]=nick[i];
            pom2=nicki[1][i];
            nicki[1][i]=pom1;
            nicki[2][i]=pom2;
        }
    }
    else if(wynik>wyn[1])
    {
        podaj_nick();
        //tutaj powinna sie wywolywac funkcja podaj nick
        int tmp1;
        tmp1=wyn[1];
        wyn[1]=wynik;
        wyn[2]=tmp1;
        char pom1;
        for(int i=0; i<11; i++)
        {
            pom1=nicki[1][i];
            nicki[1][i]=nick[i];
            nicki[2][i]=pom1;
        }
    }
    else if(wynik>wyn[2])
    {
        podaj_nick();
        //tutaj powinna sie wywolywac funkcja podaj nick
        wyn[2]=wynik;
        for(int i=0; i<11; i++)
        {
            nicki[2][i]=nick[i];
        }
    }
    for(int i=0; i<11; i++)
    {
        nick[i]=' ';
    }

    wynik=0;
}
