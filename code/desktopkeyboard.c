#include <stdio.h>
#include <windows.h>
#pragma execution_character_set( "utf-8" )

#define WIDTH   74
#define HEIGHT  6

#define arraylen(x) (sizeof(x)/sizeof(x[0]))

int hidecursor()
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    GetConsoleCursorInfo(consoleHandle,&info);
    info.bVisible = FALSE;
    return SetConsoleCursorInfo(consoleHandle,&info);
}

unsigned char   g_font_color = 0x07;

unsigned char   g_front_screen_index = 0,g_back_screen_index = 1;
/* XXXX XXXX XXXX XXXX */
unsigned short  g_screen_buffer[2][25*80] = {0};

int clearscreen()
{
    memset(g_screen_buffer[g_back_screen_index],0,sizeof(g_screen_buffer[g_back_screen_index]));
}

int printxyf(int x,int y,char * s,...){
    char buf[255];
    int ret;
    va_list va;
    va_start(va,s);
    ret = vsprintf(buf,s,va);
    va_end(va);

    for(int i=0;i < strlen(buf);i++){
        g_screen_buffer[g_back_screen_index][y*80+x+i] = buf[i] + (g_font_color << 8);
    }
    return ret;
}

int swapbuffer(){
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    for(int i=0;i<6*80;i++){
        if(g_screen_buffer[g_front_screen_index][i] != g_screen_buffer[g_back_screen_index][i]){
            COORD coord = {i%80,i/80};
            SetConsoleCursorPosition(consoleHandle,coord);
            SetConsoleTextAttribute(consoleHandle,(g_screen_buffer[g_back_screen_index][i] >> 8));
            printf("%c",g_screen_buffer[g_back_screen_index][i]);
            g_screen_buffer[g_front_screen_index][i] = g_screen_buffer[g_back_screen_index][i];
        }
    }
    g_front_screen_index    ^= g_back_screen_index;
    g_back_screen_index     ^= g_front_screen_index;
    g_front_screen_index    ^= g_back_screen_index;
}

int setcolor(int c){
    g_font_color = c;
}

int keydown(int k){
    return GetAsyncKeyState(k)&0x8000;
}

int title(char *s){
    return SetConsoleTitleA(s);
}

/*
0  345  89
[Esc][F1][F2][F3][F4][F5][F6][F7][F8][F9][F0]
[`][1][2][3][4][5][6][7][8][9][0][-][=][Back]
[Tab][Q][W][E][R][T][Y][U][I][O][P][[][]][|\]
[Caps][A][S][D][F][G][H][J][K][L][;]['][<Ent]
[Shift^][Z][X][C][V][B][N][M][,][.][/][Shift]
[Ctrl][Win][Alt][   Space   ][Alt][Win][Ctrl]

[Esc][F1][F2][F3][F4][F5][F6][F7][F8][F9][F10][F11][F12][Pau]
[`][1][2][3][4][5][6][7][8][9][0][-][=][Back] [Hom][Pgu][Ins] [l][/][*][-]
[Tab][Q][W][E][R][T][Y][U][I][O][P][[][]][ \] [End][Pgd][Del] [7][8][9][+]
[Caps][A][S][D][F][G][H][J][K][L][;]['][<Ret]                 [4][5][6][+]
[Shift^][Z][X][C][V][B][N][M][,][.][/][Shift]      [ ^ ]      [1][2][3][e]
[Ctrl][Win][Alt][   Space   ][Alt][Win][Ctrl] [ < ][ v ][ > ] [ 0  ][.][n]
*/

struct KEY{
    char * name;
    int key;
    int y;
    int x;
    struct KEY * next;
};
struct KEY keyhead = {
    0,0,0,0,
    NULL
};



print(){
    clearscreen();

    int id = 0;
    int defcolor = 0x08;
    int keycolor = 0x0F;
    int higcolor = 0x0C;

    struct KEY* p = NULL;
    for(p = keyhead.next; p!=NULL; p=p->next){
        setcolor(defcolor);
        int x = p->x;
        int y = p->y;
        int k = p->key;
        char * n = p->name;
        x += printxyf(x,y,"[");

        if(keydown(k)){
            setcolor(higcolor);
        }else{
            setcolor(keycolor);
        }
        x += printxyf(x,y,"%s",n);
        setcolor(defcolor);
        x += printxyf(x,y,"]");
    }
}


int RegKey(char *name,int id,int y,int x){
    struct KEY* key = malloc(sizeof (struct KEY));
    if(!key)return -1;
    key->key = id;
    key->name = name;
    key->x  = x;
    key->y  = y;
    key->next   = NULL;

    struct KEY* p = NULL;
    int ret = 0;
    for(p = &keyhead; p->next!=NULL; p=p->next){
        ret++;
    }
    p->next = key;
    return ret+1;
}


init(){
    RegKey("Esc",   VK_ESCAPE,  0,0  );
    RegKey("F1",    VK_F1,      0,5  );
    RegKey("F2",    VK_F2,      0,9  );
    RegKey("F3",    VK_F3,      0,13 );
    RegKey("F4",    VK_F4,      0,17 );
    RegKey("F5",    VK_F5,      0,21 );
    RegKey("F6",    VK_F6,      0,25 );
    RegKey("F7",    VK_F7,      0,29 );
    RegKey("F8",    VK_F8,      0,33 );
    RegKey("F9",    VK_F9,      0,37 );
    RegKey("F10",   VK_F10,     0,41 );
    
    RegKey("F11",   VK_F11,     0,46 );
    RegKey("F12",   VK_F12,     0,51 );
    RegKey("Pau",   VK_PAUSE,   0,56 );
    
    RegKey("`",     VK_OEM_3,   1,0  );
    RegKey("1",     '1',        1,3  );
    RegKey("2",     '2',        1,6  );
    RegKey("3",     '3',        1,9  );
    RegKey("4",     '4',        1,12 );
    RegKey("5",     '5',        1,15 );
    RegKey("6",     '6',        1,18 );
    RegKey("7",     '7',        1,21 );
    RegKey("8",     '8',        1,24 );
    RegKey("9",     '9',        1,27 );
    RegKey("0",     '0',        1,30 );
    RegKey("-",     VK_OEM_MINUS,1,33);
    RegKey("=",     VK_OEM_PLUS,1,36 );
    RegKey("Back",  VK_BACK,    1,39 );

    RegKey("Tab",   VK_TAB,     2,0  );
    RegKey("Q",     'Q',        2,5  );
    RegKey("W",     'W',        2,8  );
    RegKey("E",     'E',        2,11 );
    RegKey("R",     'R',        2,14 );
    RegKey("T",     'T',        2,17 );
    RegKey("Y",     'Y',        2,20 );
    RegKey("U",     'U',        2,23 );
    RegKey("I",     'I',        2,26 );
    RegKey("O",     'O',        2,29 );
    RegKey("P",     'P',        2,32 );
    RegKey("[",     VK_OEM_4,   2,35 );
    RegKey("]",     VK_OEM_6,   2,38 );
    RegKey(" \\",   VK_OEM_5,   2,41 );

    RegKey("Caps",  VK_CAPITAL, 3,0  );
    RegKey("A",     'A',        3,6  );
    RegKey("S",     'S',        3,9  );
    RegKey("D",     'D',        3,12 );
    RegKey("F",     'F',        3,15 );
    RegKey("G",     'G',        3,18 );
    RegKey("H",     'H',        3,21 );
    RegKey("J",     'J',        3,24 );
    RegKey("K",     'K',        3,27 );
    RegKey("L",     'L',        3,30 );
    RegKey(";",     VK_OEM_1,   3,33 );
    RegKey("'",     VK_OEM_7,   3,36 );
    RegKey("Ent ",  VK_RETURN,  3,39 );
    RegKey("<Ret",  VK_RETURN,  3,39 );

    RegKey("Shift^",VK_LSHIFT,  4,0  );
    RegKey("Z",     'Z',        4,8  );
    RegKey("X",     'X',        4,11 );
    RegKey("C",     'C',        4,14 );
    RegKey("V",     'V',        4,17 );
    RegKey("B",     'B',        4,20 );
    RegKey("N",     'N',        4,23 );
    RegKey("M",     'M',        4,26 );
    RegKey(",",     VK_OEM_COMMA,4,29);
    RegKey(".",     VK_OEM_PERIOD,4,32);
    RegKey("/",     VK_OEM_2,   4,35 );
    RegKey("Shift", VK_RSHIFT,  4,38 );


    RegKey("Ctrl",  VK_LCONTROL,5,0  );
    RegKey("Win",   VK_LWIN,    5,6  );
    RegKey("Alt",   VK_LMENU,   5,11 );
    RegKey("   Space   ",VK_SPACE,5,16);
    RegKey("Alt",   VK_RMENU,   5,29 );
    RegKey("Win",   VK_RWIN,    5,34 );
    RegKey("Ctrl",  VK_RCONTROL,5,39 );

    RegKey(" < ",   VK_LEFT,    5,46 );
    RegKey(" v ",   VK_DOWN,    5,51 );
    RegKey(" > ",   VK_RIGHT,   5,56 );
    RegKey(" ^ ",   VK_UP,      4,51 );

    RegKey("Pgu",   VK_PRIOR,   1,51 );
    RegKey("Pgd",   VK_NEXT,    2,51 );
    RegKey("Hom",   VK_HOME,    1,46 );
    RegKey("End",   VK_END,     2,46 );
    RegKey("Ins",   VK_INSERT,  1,56 );
    RegKey("Del",   VK_DELETE,  2,56 );

    RegKey(" 0  ",  VK_NUMPAD0, 5,62 );
    RegKey(".",     VK_DECIMAL, 5,68 );
    RegKey("1",     VK_NUMPAD1, 4,62 );
    RegKey("2",     VK_NUMPAD2, 4,65 );
    RegKey("3",     VK_NUMPAD3, 4,68 );
    RegKey("4",     VK_NUMPAD4, 3,62 );
    RegKey("5",     VK_NUMPAD5, 3,65 );
    RegKey("6",     VK_NUMPAD6, 3,68 );
    RegKey("7",     VK_NUMPAD7, 2,62 );
    RegKey("8",     VK_NUMPAD8, 2,65 );
    RegKey("9",     VK_NUMPAD9, 2,68 );

    RegKey("l",     VK_NUMLOCK, 1,62 );
    RegKey("/",     VK_DIVIDE,  1,65 );
    RegKey("*",     VK_MULTIPLY,1,68 );
    RegKey("-",     VK_SUBTRACT,1,71 );
    RegKey("+",     VK_ADD,     2,71 );
    RegKey("+",     VK_ADD,     3,71 );
    RegKey("e",     VK_RETURN,  4,71 );
    //RegKey("n",     VK_RETURN,  5,71 );

}

void SetWindowSize(int w,int h)
{
    SMALL_RECT test; 
    HANDLE hStdout;
    COORD coord;

    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    coord.X = w;
    coord.Y = h;

    test.Left = 0;
    test.Top = 0;
    test.Right = coord.X-1;
    test.Bottom = coord.Y-1;

    SetConsoleWindowInfo(hStdout, 1, &test);
    SetConsoleScreenBufferSize(hStdout, coord);
}

CenterWindow(){
    int sw = GetSystemMetrics(SM_CXSCREEN);
    int sh = GetSystemMetrics(SM_CYSCREEN);
    HWND hWnd = GetConsoleWindow();
    RECT rt = {0};

    GetWindowRect(hWnd,&rt);
    int w = rt.right-rt.left;
    int h = rt.bottom-rt.top;

    SetWindowPos(hWnd,HWND_TOPMOST,(sw-w)/2,(sh-h)/2,w,h,SWP_NOMOVE | SWP_NOSIZE);
    MoveWindow(hWnd,(sw-w)/2,(sh-h),w,h,TRUE);
}

info() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int columns, rows;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    printf("columns: %d\n", columns);
    printf("rows: %d\n", rows);
    getchar();
}

Debug(){
    getchar();
    int i;
    for(i=0;i<255;i++){
        if(GetAsyncKeyState(i)&0x8000)
            printf("0x%X\n",i);
    }
    getchar();
}

main(){
    //Debug();
    SetWindowSize(74,6);
    CenterWindow();
    hidecursor();
    //info();

    title("keyboard ver 0.5");
    init();

    while(1){
        print();
        swapbuffer();
        Sleep(40);
    }
}