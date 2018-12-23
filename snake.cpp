#include<windows.h>
#include<cstdio>
#include<vector>
#include<random>

LRESULT CALLBACK WndProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp);
void setEsa();

const int
	stwh=10,    // 縦横のマスの数
	dotSize=40, // 1ドットの大きさ
	colbody=3,
	colhead=13;
	
const short mvin[5][2]={{0,0},{-1,0},{0,-1},{1,0},{0,1}};


int
	table[stwh][stwh],
	atari[stwh][stwh],
	into=0,
	leng=1,
	npos[2]={stwh/2,stwh/2},
	esa[2];

std::random_device rnd;

int tm;

bool
	gameover,
	tick,
	ind;

void reset(){
	into=0;
	leng=1;
	npos[0]=stwh/2;
	npos[1]=stwh/2;
	tm=0;
	ind=true;
	gameover=false;
	tick=false;
	setEsa();
	for(int i=0;i<stwh;i++){
		for(int j=0;j<stwh;j++){
			table[i][j]=(i==npos[0] and j==npos[1])-(i==esa[0] and j==esa[1]);
		}
	}
	printf("===============\ntlllune HIMAHIMA products #0\nSnake Game\n-------\n[arrow keys] : control snake\n[q] : quit(restart)\n===============\n\n");
}

int formpos(int pos){
	return (pos+(pos<0)*stwh)*(pos<stwh);
}

int mx(int a,int b){
	if(a<b)return b;
	else return a;
}

int mht(int xp,int yp,int xtp,int ytp){
    return abs(xp-xtp)+abs(yp-ytp);
}


void setEsa(){
	if(leng==stwh*stwh){
		esa[0]=-1;
		esa[1]=-1;
		return;
	}
	std::vector<int> ix={},iy={};
	
	for(int i=0;i<stwh;i++){
		for(int j=0;j<stwh;j++){
			atari[i][j]=0;
			if(table[i][j]>0){
				iy.push_back(i);
				ix.push_back(j);
			}
		}
	}

	int colider,fa,fb,mnat;

	for(int m=0;m<ix.size();m++){
		if(iy[m]==npos[0] and ix[m]==npos[1])colider=colhead;
		else colider=colbody;
		for(int i=-colider;i<=colider;i++){
			for(int j=-colider;j<=colider;j++){
				fa=formpos(ix[m]+j);
				fb=formpos(iy[m]+i);
				atari[fa][fb]=mx(mx(colider-abs(i)-abs(j),0),atari[fa][fb]);
			}
		}
	}
	std::vector<std::vector<int>> vx,vy;
	for(int i=0;i<=mx(colbody,colhead);i++){
		vx.push_back({});
		vy.push_back({});
	}
	mnat=atari[0][0];

	for(int i=0;i<stwh;i++){
		for(int j=0;j<stwh;j++){
			vx[atari[i][j]].push_back(i);
			vy[atari[i][j]].push_back(j);
			mnat=mnat>atari[i][j] ? atari[i][j]:mnat;
		}
	}

	int rndq=rnd()%vx[mnat].size();
	esa[0]=vy[mnat][rndq];
	esa[1]=vx[mnat][rndq];
	
	table[esa[0]][esa[1]]=-1;


}

void gomessage(){
	printf("Game Over\nyour Score  ...  %d\npress [q] to restart\n\n",leng);
}

void movein(){
	
	npos[0]=formpos(npos[0]+mvin[into][0]);
	npos[1]=formpos(npos[1]+mvin[into][1]);

	if(into!=0 and table[npos[0]][npos[1]]>1){
		gameover=true;
		gomessage();
	}
	else{
		if(npos[0]==esa[0] and npos[1]==esa[1]){
			leng++;
			setEsa();
		}
		else{
			for(int i=0;i<stwh;i++){
				for(int j=0;j<stwh;j++){
					table[i][j]-=(table[i][j]!=0);
				}
			}
		}
		table[npos[0]][npos[1]]=leng;
	}
	ind=true;
	
}


int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,PSTR lpCmdLine,int nCmdShow) {
	HWND hwnd;
	MSG msg;
	WNDCLASS winc;
	
	reset();
	
	winc.style = CS_HREDRAW | CS_VREDRAW;
	winc.lpfnWndProc = WndProc;
	winc.cbClsExtra	= winc.cbWndExtra = 0;
	winc.hInstance = hInstance;
	winc.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	winc.hCursor = LoadCursor(NULL,IDC_ARROW);
	winc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	winc.lpszMenuName = NULL;
	winc.lpszClassName = TEXT("MAIN");

	if (!RegisterClass(&winc)) return 0;

	hwnd = CreateWindowEx(
			WS_EX_COMPOSITED,TEXT("MAIN"),TEXT("SNAKE GAME"),
			WS_VISIBLE,
			100,100,stwh*dotSize+1,stwh*dotSize+1,NULL,NULL,hInstance,NULL
	);

	SetWindowLong(hwnd, GWL_STYLE,WS_VISIBLE | WS_BORDER);
	if (hwnd==NULL) return 0;
	
	
	while(GetMessage(&msg,NULL,0,0)){
		
		ReleaseCapture();
		SendMessage(hwnd, WM_SYSCOMMAND, SC_MOVE | 2,0);
		
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp){

	HDC hdc;
	PAINTSTRUCT ps;
	int colh=tick*255;
	

	switch(msg){
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_CREATE:
		SetTimer(hwnd,1,100,NULL);
		return 0;
	case WM_TIMER:

		if(!gameover){
			movein();
		}
		else{
			tick=!tick;
		}
		if(into!=0 and leng==stwh*stwh and !gameover){
			gameover=true;
			printf("Congratulations! C O M P L E T E\n");
			gomessage();
		}
		InvalidateRect(hwnd,NULL,true);
		//UpdateWindow(hwnd);
		return 0;

	case WM_PAINT:
		
		hdc=BeginPaint(hwnd,&ps);
		SelectObject(hdc,CreateSolidBrush(RGB(colh,colh,colh)));
		for(int i=0;i<stwh;i++){
			for(int j=0;j<stwh;j++){
				if(table[i][j]){
					if(table[i][j]>0){
						PatBlt(hdc,i*dotSize,j*dotSize,dotSize,dotSize,PATCOPY);
					}
					else{
						DeleteObject(SelectObject(hdc,GetStockObject(WHITE_BRUSH)));
						SelectObject(hdc,CreateSolidBrush(RGB(255,0,0)));
						PatBlt(hdc,i*dotSize,j*dotSize,dotSize,dotSize,PATCOPY);
						DeleteObject(SelectObject(hdc,GetStockObject(WHITE_BRUSH)));
						SelectObject(hdc,CreateSolidBrush(RGB(colh,colh,colh)));
					}
					
				}
				//printf("%d ",table[i][j]);
			}
			//printf("\n");
		}
		
		DeleteObject(SelectObject(hdc,GetStockObject(WHITE_BRUSH)));
		EndPaint(hwnd , &ps);
		
		return 0;
	case WM_KEYDOWN:
		switch(wp){
		case 81:
			if(!gameover)PostQuitMessage(0);
			else{
				reset();
			}
			return 0;
		case 37:
		case 38:
		case 39:
		case 40:
			if((into%2!=wp%2 or into==0 or leng==1) and ind){
				into=wp-36;
				ind=false;
			}
			return 0;
		default:return 0;
		}
		
	}
	return DefWindowProc(hwnd,msg,wp,lp);
}