#include "plansza.h"

bool tura = 0;
char moje_statki[12][12];       // '0' - pusto, '1' - statek, '?' - nie sprawdzone pole
char statki_przeciwnika[12][12];
// wywołać raz w main'ie  przed grą żeby zdefiniować statki
// przed użyciem funckji  void uzupelnanie_moich_statow(int X, int Y)
void inicjalizacja_plansz(){
  for(int i=0 ;i<12; i++)
    for(int j=0; j<12; j++)
      moje_statki[i][j]='0';

  for(int i=0 ;i<12; i++)
    for(int j=0; j<12; j++)
      statki_przeciwnika[i][j]='?';
}

void rysuj_linie(const int x1, const int y1, const int x2, const int y2, uint16_t col){
  int d, dx, dy, ai, bi, xi, yi;
  int x = x1, y = y1;
  if (x1 < x2){
    xi = 1;
    dx = x2 - x1;
  }
  else{
    xi = -1;
    dx = x1 - x2;
  }
  if (y1 < y2){
    yi = 1;
    dy = y2 - y1;
  }
  else{
    yi = -1;
    dy = y1 - y2;
  }
    lcdWriteReg(ADRX_RAM, x);
    lcdWriteReg(ADRY_RAM, y);
    lcdWriteReg(DATA_RAM, col);

    if (dx > dy)
    {
        ai = (dy - dx) * 2;
        bi = dy * 2;
        d = bi - dx;
        while (x != x2)
        {
            if (d >= 0)
            {
                x += xi;
                y += yi;
                d += ai;
            }
            else
            {
                d += bi;
                x += xi;
            }
            lcdWriteReg(ADRX_RAM, x);
            lcdWriteReg(ADRY_RAM, y);
            lcdWriteReg(DATA_RAM, col);
        }
    }
    else
    {
        ai = (dx - dy) * 2;
        bi = dx * 2;
        d = bi - dy;
        while (y != y2)
        {
            if (d >= 0)
            {
                x += xi;
                y += yi;
                d += ai;
            }
            else
            {
                d += bi;
                y += yi;
            }
            lcdWriteReg(ADRX_RAM, x);
            lcdWriteReg(ADRY_RAM, y);
            lcdWriteReg(DATA_RAM, col);
        }
    }
}

void rysuj_kwadrat(int kolor,int x,int y, int szer){
  // x, y - lewy górny róg 
  // szer - szerokosć kwadratu

		lcdSetCursor(x,y);
	lcdWriteIndex(DATA_RAM);
  for(int i=0; i<szer; i++)
  {
    lcdSetCursor(x,y+i);
	lcdWriteIndex(DATA_RAM);
    for(int j=0; j<szer; j++)
    {
      lcdWriteData(kolor);
    }
  }

}


void rysuj_plansze(char tab_statki[12][12]){
  for(int i=1; i<11; i++){
    for(int j=1; j<11; j++){
      if(tab_statki[i][j] == '0') // brak statku
        rysuj_kwadrat(LCDBlue,i*20,j*20,20);
      if(tab_statki[i][j] == '1') // statek
        rysuj_kwadrat(LCDRed,i*20,j*20,20);
      if(tab_statki[i][j] == '?') // nie sprawdzone pole (nie strzelano)
        rysuj_kwadrat(LCDWhite,i*20,j*20,20);
    }
  }
	for(int i=0; i<=10 ; i++)
	rysuj_linie(20+i*20,20,20+i*20,220,LCDBlack);
	for(int j=0; j<=10; j++)
	rysuj_linie(20,20*j+20,220,20*j+20,LCDBlack);
  
}

// (20,20), (220,220) przekatne  200 x 200 - całość; 20 x 20 - jedna pozycja  

void LCD_to_index(int *x, int *y){
  // X, Y -> x_id, y_id
  // sprawdzenie czy jest poza planszą
  *x = (*x-20)/20 + 1;
  *y = (*y-20)/20 + 1;
}


// Rysuje statki poniżej planszy
void rysuj_rodzaje_statkow(){
  // 10 px na 10 px 
  //4 pionowe
  int x1= 240; // do 320 , 300, 280 , 260
  int y1= 20; // do 40
  int y2= 50; // do 700
  int y3= 80; // do 100
  int y4= 110; // do 130
  
  //4m poziomy
  int y_poz1 =140; //do 220
  int x_poz1 = 240; // do 260
  //3m poziomy
  int y_poz2 =160; //do 220
  int x_poz3 =270 // do 290
  //poziomy dwómasztowiec
  int x_poz2 = 270;	// do 290
  int y_poz3 = 110;	// do 150

  // 4m_pion
  for(int i=0; i<4; i++)
    rysuj_kwadrat(LCDRed,y1,x1+i*20,20);
  // 3m_pion
  for(int i=0; i<3; i++)
    rysuj_kwadrat(LCDRed,y2,x1+i*20,20);
  // 2m_pion
  for(int i=0; i<2; i++)
    rysuj_kwadrat(LCDRed,y3,x1+i*20,20);
  // 1m
    rysuj_kwadrat(LCDRed,y4,x1,20);
  
  // 4m_poziom
  for(int i=0; i<4; i++)
    rysuj_kwadrat(LCDRed,y_poz1+i*20,x_poz1,20);
  // 3m_poziom
  for(int i=0; i<3; i++)
    rysuj_kwadrat(LCDRed,y_poz2+i*20,x_poz3,20);
  // 2m_poziom
  for(int i=0; i<2; i++)
    rysuj_kwadrat(LCDRed,y_poz3+i*20,x_poz2,20);
}

int wybor_statku(){
  // pobiera warość kliknięcia
  // zamienia ją na pozycje jednego ze statków do wyboru
  // pozycje znajduą się w funckji rysuj statki
  // zwraca numer statku
  int toReturn = 0;

  while(flaga == 0);
  touchpad_to_LCD_XY();
  if(Y>=240 && Y<= 320 && X>=20 && X<=40)
    toReturn = 2;
  else if(Y>= 240 && Y<=300 && X>=50 && X<=70)
    toReturn = 4;
  else if(Y>= 240 && Y<=280 && X>=80 && X<=100)
    toReturn = 6;
  else if(Y>= 240 && Y<=260 && X>=110 && X<=130)
    toReturn = 7;
  else if(Y>=240 && Y<=260 && X>=140 && X<=220)
    toReturn = 1;
  else if(Y>=270 && Y<=290 && X>=160 && X<=220)
    toReturn = 3;
  else if(Y>=270 && Y<=290 && X>=110 && X<=150)
    toReturn = 5;
  while(flaga == 1);

  return toReturn;
}

void uzupelnanie_moich_statow(){
  // x_id, y_id - odnosi sie do indeksów tabeli moje_statki
  // x_id góra dół ( a,b,c,d...)
  // y_id lewo prawo ( 0,1,2,3,4...)
  int x_id;
  int y_id;
  
  int postawione_statki[4] = {4,3,2,1}; // wszystkie statki postawione - {0,0,0,0}

  // while wstawić do main, a funkcja uzupelnanie_moich_statow(int X, int Y)
  // zawartość whila dać do tej funkcji  uzupelnanie_moich_statow(int X, int Y)
  
  while(postawione_statki[0]!=0 || postawione_statki[1]!=0 || postawione_statki[2] !=0 || postawione_statki[3] != 0){

    // wybór rodzaju statku
    // wybór miejsca
    int stawiany_statek = 0;  // wyświetlimy poniżej do wyboru
    // 1 - 4m_pion
    // 2 - 4m_poziom
    // 3 - 3m_pion
    // 4 - 3m_poziom
    // 5 - 2m_pion
    // 6 - 2m_poziom
    // 7 - 1m

    rysuj_rodzaje_statkow();  
    napis(100,20,"Wybierz statek",14);
		char strx[4];
    while(stawiany_statek == 0){
      stawiany_statek = wybor_statku(); // zwraca warość wybranego statku
			//sprintf(strx,"%d",(int)(stawiany_statek));
			//wypisz(strx,4);
    }
    // wybór_miejsca // X, Y -> X_LCD, Y_LCD -> x_id, y_id
    
    rysuj_plansze(moje_statki);
    napis(240,20,"Wybierz miejsce",15);

    while(flaga == 0);
      // przypisanie nowych X i Y (w przerwaniu) 
    touchpad_to_LCD_XY();
		x_id=X;
		y_id=Y;
    LCD_to_index(&x_id,&y_id);
		//sprintf(strx,"x=%d",(int)(x_id));
			//wypisz(strx,4);
			//sprintf(strx,"y=%d",(int)(y_id));
			//wypisz(strx,4);
	  while(flaga == 1);

    // int x_LCD = X, y_LCD = Y; // do rysuj_statek
    // int x_id = X, y_id = Y; // do zapisywania w tablicy

	
    bool dobre_miejsce = 1;
    switch(stawiany_statek){
      case 1:{

          if(x_id>7 || postawione_statki[3] == 0){
            wypisz("zle miejsce",11);
            dobre_miejsce = 0;  
          }
          else{
            for(int i= x_id-1; i<=x_id+4;i++){
              for(int j= y_id-1; j<=y_id+1;j++){
                if(moje_statki[i][j] == '1'){
                  dobre_miejsce = 0;
                  break;
                }
              }
            }
            if(dobre_miejsce == 1){
              moje_statki[x_id][y_id]  ='1'; 
              moje_statki[x_id+1][y_id]='1';  
              moje_statki[x_id+2][y_id]='1';  
              moje_statki[x_id+3][y_id]='1';  
              postawione_statki[3]--;
              // rysuj_statek(x,y)
            }
          }
        break;
      }
      case 2:{
          if(y_id>7 || postawione_statki[3] == 0){
            wypisz("zle miejsce",11);
            dobre_miejsce = 0;  
          }
          else{
            for(int i= x_id-1; i<=x_id+1;i++){
              for(int j= y_id-1; j<=y_id+4;j++){
                if(moje_statki[i][j] == '1'){
                  dobre_miejsce = 0;
                  break;
                }
              }
            }
            if(dobre_miejsce == 1){
              moje_statki[x_id][y_id]  ='1'; 
              moje_statki[x_id][y_id+1]='1'; 
              moje_statki[x_id][y_id+2]='1'; 
              moje_statki[x_id][y_id+3]='1';  
              postawione_statki[3]--;
            }
          }
        break;
      }
      case 3:{
          if(x_id>8 || postawione_statki[2] == 0){
            wypisz("zle miejsce",11);
            dobre_miejsce = 0;  
          }
          else{
            for(int i= x_id-1; i<=x_id+3;i++){
              for(int j= y_id-1; j<=y_id+1;j++){
                if(moje_statki[i][j] == '1'){
                  dobre_miejsce = 0;
                  break;
                }
              }
            }
            if(dobre_miejsce == 1){
              moje_statki[x_id][y_id]  ='1'; 
              moje_statki[x_id+1][y_id]='1';  
              moje_statki[x_id+2][y_id]='1';  
              postawione_statki[2]--;
            }
          }
        break;
      }
          case 4:{
          if(y_id>8 || postawione_statki[2] == 0){
            wypisz("zle miejsce",11);
            dobre_miejsce = 0;  
          }
          else{
            for(int i= x_id-1; i<=x_id+1;i++){
              for(int j= y_id-1; j<=y_id+3;j++){
                if(moje_statki[i][j] == '1'){
                  dobre_miejsce = 0;
                  break;
                }
              }
            }
            if(dobre_miejsce == 1){
              moje_statki[x_id][y_id]  ='1'; 
              moje_statki[x_id][y_id+1]='1';  
              moje_statki[x_id][y_id+2]='1'; 
              postawione_statki[2]--;
            }
          }
        break;
      }
      case 5:{
          if(x_id>9 || postawione_statki[1] == 0){
            wypisz("zle miejsce",11);
            dobre_miejsce = 0;  
          }
          else{
            for(int i= x_id-1; i<=x_id+2;i++){
              for(int j= y_id-1; j<=y_id+1;j++){
                if(moje_statki[i][j] == '1'){
                  dobre_miejsce = 0;
                  break;
                }
              }
            }
            if(dobre_miejsce == 1){
              moje_statki[x_id][y_id]  ='1';  
              moje_statki[x_id+1][y_id]='1';  

              postawione_statki[1]--;
            }
          }
        break;
      }
    case 6:{
      if(y_id>9 || postawione_statki[1] == 0){
        wypisz("zle miejsce",11);
        dobre_miejsce = 0;  
      }
      else{
        for(int i= x_id-1; i<=x_id+1;i++){
          for(int j= y_id-1; j<=y_id+2;j++){
            if(moje_statki[i][j] == '1'){
              dobre_miejsce = 0;
                break;
            }
          }

        }
        if(dobre_miejsce == 1){
          moje_statki[x_id][y_id]  ='1'; 
          moje_statki[x_id][y_id+1]='1'; 
          postawione_statki[1]--;
        }
      }
      break;
    }  
      case 7:{
          if(x_id>10 || postawione_statki[0] == 0){
            wypisz("zle miejsce",11);
            dobre_miejsce = 0;  
          }
          else{
            for(int i= x_id-1; i<=x_id+1;i++)
              for(int j= y_id-1; j<=y_id+1;j++)
                if(moje_statki[i][j] == '1'){
                  dobre_miejsce = 0;
                  break;
                }
            if(dobre_miejsce == 1){
              moje_statki[x_id][y_id]  ='1'; 
              postawione_statki[0]--;
            }
          }
        break;
      }
    
  }
		  rysuj_plansze(moje_statki);
}
	

uint8_t info;

	wyslij_info(KTO_PIERWSZY);
	info = LPC_UART0->RBR;
  if( info == KTO_PIERWSZY){
		tura=0;
	}
	else{
    tura=1;
  } 
	//char tempo=48+tura;
	//napis(42,42,&tempo,1);

}


