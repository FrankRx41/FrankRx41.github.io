#include <windows.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

#define debug(str,...)  printf(str"\n",__VA_ARGS__)

#define IDM_RED     50001
#define IDM_BLACK   50002
#define IDM_EXIT    50003
#define IDM_DEBUG   50010
#define IDM_NODEBUG 50011

#define ID_EDIT     10001

#define CHESS_SIZE  (50)
#define FONT_SIZE   (30)
#define CHAR_SIZE   (50)
#define WIDTH   (CHESS_SIZE*9)
#define HEIGHT  (CHESS_SIZE*10+CHAR_SIZE*2)
#define TEXT_WIDTH  (200)

#define BLACK   (1)
#define RED     (0)

#ifdef _DEBUG
#define DEBUG
#endif

HWND hWnd;
HWND hText;
/*
    mdc 只會在繪製棋的時候使用，不要考慮相對窗口的位置
    bgdc 只會在初始的時候繪製一次
*/
HDC     mdc;
HDC     bgdc;
HFONT   fontlisu;


int turn = 0;

typedef struct tagCHESS{
    unsigned int x:4;
    unsigned int y:4;
    unsigned int owner:1;
    TCHAR text[2];
}CHESS;

typedef struct tagCHESSNODE{
    CHESS chess;
    struct tagCHESSNODE * next;
}CHESSNODE;
CHESSNODE chesshead = {0};
CHESSNODE chessway = {0};

int DrawChessField(HDC mdc){
    int w = CHESS_SIZE*9;
    int h = CHESS_SIZE*10;
    int i;
    HPEN pen = CreatePen(PS_SOLID,2,RGB(0,0,0)),oldpen;
    RECT rt = {0,0,WIDTH,HEIGHT};
    HBRUSH brush = CreateSolidBrush(RGB(175,129,79));
    HFONT oldfont,font1,font2;
    int padding = 5;
    
    /*
        填充背景色
    */
    FillRect(mdc,&rt,brush);
    /*
        繪製上方區域的豎
    */
    for(i=CHESS_SIZE/2; i<w; i+=CHESS_SIZE){
        MoveToEx(mdc,   i,  CHAR_SIZE+CHESS_SIZE/2,NULL);
        LineTo(mdc,     i,  CHAR_SIZE+h/2-CHESS_SIZE/2);
    }
    /*
        繪製下方的區域豎
    */
    for(i=CHESS_SIZE/2; i<w; i+=CHESS_SIZE){
        MoveToEx(mdc,   i,  CHAR_SIZE+h/2+CHESS_SIZE/2,NULL);
        LineTo(mdc,     i,  CHAR_SIZE+h-CHESS_SIZE/2);
    }
    /*
        繪製上方區域和下方區域的橫
    */
    for(i=CHESS_SIZE/2; i<h; i+=CHESS_SIZE){
        MoveToEx(mdc,   CHESS_SIZE/2,   CHAR_SIZE+i,NULL);
        LineTo(mdc,     w-CHESS_SIZE/2, CHAR_SIZE+i);
    }
    /*
        繪製上方區域九宮格的斜線
    */
    MoveToEx(mdc,   CHESS_SIZE*3+CHESS_SIZE/2,CHAR_SIZE+CHESS_SIZE/2,NULL);
    LineTo(mdc,     CHESS_SIZE*5+CHESS_SIZE/2,CHAR_SIZE+CHESS_SIZE*2+CHESS_SIZE/2);
    MoveToEx(mdc,   CHESS_SIZE*5+CHESS_SIZE/2,CHAR_SIZE+CHESS_SIZE/2,NULL);
    LineTo(mdc,     CHESS_SIZE*3+CHESS_SIZE/2,CHAR_SIZE+CHESS_SIZE*2+CHESS_SIZE/2);
    /*
        繪製下方區域九宮格的斜線
    */
    MoveToEx(mdc,   CHESS_SIZE*3+CHESS_SIZE/2,CHAR_SIZE+h-CHESS_SIZE/2,NULL);
    LineTo(mdc,     CHESS_SIZE*5+CHESS_SIZE/2,CHAR_SIZE+h-CHESS_SIZE*2-CHESS_SIZE/2);
    MoveToEx(mdc,   CHESS_SIZE*5+CHESS_SIZE/2,CHAR_SIZE+h-CHESS_SIZE/2,NULL);
    LineTo(mdc,     CHESS_SIZE*3+CHESS_SIZE/2,CHAR_SIZE+h-CHESS_SIZE*2-CHESS_SIZE/2);
    /*
        TODO: 繪製炮的輔助線
    */

    /*
        關鍵線加粗
    */
    oldpen = (HPEN)SelectObject(mdc,pen);
    MoveToEx(mdc,   CHESS_SIZE/2,   CHAR_SIZE+CHESS_SIZE/2,NULL);
    LineTo(mdc,     CHESS_SIZE/2,   CHAR_SIZE+h-CHESS_SIZE/2);
    LineTo(mdc,     w-CHESS_SIZE/2, CHAR_SIZE+h-CHESS_SIZE/2);
    LineTo(mdc,     w-CHESS_SIZE/2, CHAR_SIZE+CHESS_SIZE/2);
    LineTo(mdc,     CHESS_SIZE/2,   CHAR_SIZE+CHESS_SIZE/2);

    MoveToEx(mdc,   CHESS_SIZE/2,   CHAR_SIZE+h/2-CHESS_SIZE/2,NULL);
    LineTo(mdc,     w-CHESS_SIZE/2, CHAR_SIZE+h/2-CHESS_SIZE/2);

    MoveToEx(mdc,   CHESS_SIZE/2,   CHAR_SIZE+h/2+CHESS_SIZE/2,NULL);
    LineTo(mdc,     w-CHESS_SIZE/2, CHAR_SIZE+h/2+CHESS_SIZE/2);
    /*
        繪製 楚河 漢界
    */
    SetTextColor(mdc,RGB(0,0,0));
    SetBkMode(mdc,TRANSPARENT);

    font1 = CreateFont(40,0,0,0,0,0,0,0,GB2312_CHARSET,0,0,0,FIXED_PITCH,TEXT("LiSu"));
    oldfont = (HFONT)SelectObject(mdc,font1);
    rt.left = CHESS_SIZE*1.5;
    rt.right    = rt.left + CHESS_SIZE*2;
    rt.top  = CHAR_SIZE+CHESS_SIZE*4.5;
    rt.bottom   = rt.top + CHESS_SIZE;
    DrawText(mdc,TEXT("楚河"),-1,&rt,DT_CENTER|DT_SINGLELINE|DT_VCENTER);

    font2 = CreateFont(40,0,0,0,0,0,0,0,GB2312_CHARSET,0,0,0,FIXED_PITCH,TEXT("LiSu"));
    SelectObject(mdc,font2);
    rt.left = CHESS_SIZE*5.5;
    rt.right    = rt.left + CHESS_SIZE*2;
    rt.top  = CHAR_SIZE+CHESS_SIZE*4.5;
    rt.bottom   = rt.top + CHESS_SIZE;
    DrawText(mdc,TEXT("漢界"),-1,&rt,DT_CENTER|DT_SINGLELINE|DT_VCENTER);
    /*
        繪製 數字
    */
    {
        TCHAR num[2][10] = {TEXT("123456789"),TEXT("九八七六五四三二一")};
        RECT rt = {0,0,CHESS_SIZE,CHAR_SIZE};
        HFONT numfont = CreateFont(30,0,0,0,0,0,0,0,GB2312_CHARSET,0,0,0,FIXED_PITCH,TEXT("LiSu"));
        SelectObject(mdc,numfont);

        for(i=0; i<10; i++){
            DrawText(mdc,&num[0][i],1,&rt,DT_CENTER|DT_SINGLELINE|DT_VCENTER);
            OffsetRect(&rt,CHESS_SIZE,0);
        }

        OffsetRect(&rt,-rt.left,-rt.top+HEIGHT-CHAR_SIZE);
        for(i=0; i<10; i++){
            DrawText(mdc,&num[1][i],1,&rt,DT_CENTER|DT_SINGLELINE|DT_VCENTER);
            OffsetRect(&rt,CHESS_SIZE,0);
        }
        DeleteObject(numfont);
    }
    
    SelectObject(mdc,oldfont);
    SelectObject(mdc,oldpen);
    DeleteObject(pen);
    DeleteObject(brush);
    DeleteObject(font1);
    DeleteObject(font2);
    return 0;
}

int CreateChess(int x,int y,int owner,TCHAR text){
    /*
        創建一個棋子，並且將之存放在 NODE 的末尾
    */
    CHESSNODE * chessnodefather = &chesshead;
    CHESSNODE * chessnode = (CHESSNODE *)malloc( sizeof (CHESSNODE) );
    chessnode->chess.x = x;
    chessnode->chess.y = y;
    chessnode->chess.owner = owner;
    lstrcpy(chessnode->chess.text,&text);
    chessnode->next = chessnodefather->next;
    chessnodefather->next = chessnode;
    return 0;
}

int DrawChessXY(HDC mdc,const CHESS * const chess,int x,int y){
    RECT rt = {0,0,CHESS_SIZE,CHESS_SIZE};
    HPEN pen,oldpen;
    HFONT oldfont;
    HBRUSH brush = CreateSolidBrush(RGB(250,243,150)),oldbrush;
    int padding = 2;

    OffsetRect(&rt,x,y);
    //SetBkColor(mdc,RGB(255,243,150));
    SetBkMode(mdc,TRANSPARENT);
    oldbrush = (HBRUSH)SelectObject(mdc,brush);
    Ellipse(mdc,rt.left+padding,rt.top+padding,rt.right-padding,rt.bottom-padding);

    switch(chess->owner){
    case BLACK:
        pen = CreatePen(PS_SOLID,1,RGB(0,0,0));
        SetTextColor(mdc,RGB(0,0,0));
        break;
    case RED:
        pen = CreatePen(PS_SOLID,1,RGB(255,0,0));
        SetTextColor(mdc,RGB(255,0,0));
        break;
    }
    oldpen = (HPEN)SelectObject(mdc,pen);
    oldfont = (HFONT)SelectObject(mdc,fontlisu);
    DrawText(mdc,chess->text,-1,&rt,DT_CENTER|DT_SINGLELINE|DT_VCENTER);
    SelectObject(mdc,oldpen);
    SelectObject(mdc,oldfont);
    SelectObject(mdc,oldbrush);
    DeleteObject(pen);
    DeleteObject(brush);
    return 0;
}

int DrawChess(HDC mdc,const CHESS * const chess){
    DrawChessXY(mdc,chess,chess->x*CHESS_SIZE,CHAR_SIZE+chess->y*CHESS_SIZE);
    return 0;
}

int InitChess(int t){
    /*
    TEXT("　　　　　將　　　")
    TEXT("　　兵　兵　　　　")
    TEXT("　　　　　　　　　")
    TEXT("　　兵　兵　　　　")
    TEXT("　　　　　　　　　")
    TEXT("　砲　　　　　　　")
    TEXT("　　　　　　俥　　")
    TEXT("　砲　　　　　　　")
    TEXT("　　　　　　　　　")
    TEXT("　　　　帥　俥　　")
    */
    /*
        以標準形式初始化棋盤
        以後出現 殘局 時可以通過修改這個棋盤進行
    */
    TCHAR *map = { 0 };
    int i;

    switch(t){
    case RED:
        map = 
            TEXT("車馬象士將士象馬車")
            TEXT("　　　　　　　　　")
            TEXT("　炮　　　　　炮　")
            TEXT("卒　卒　卒　卒　卒")
            TEXT("　　　　　　　　　")
            TEXT("　　　　　　　　　")
            TEXT("兵　兵　兵　兵　兵")
            TEXT("　砲　　　　　砲　")
            TEXT("　　　　　　　　　")
            TEXT("俥傌相仕帥仕相傌俥");
        break;
    case BLACK:
        map = 
            TEXT("俥傌相仕帥仕相傌俥")
            TEXT("　　　　　　　　　")
            TEXT("　砲　　　　　砲　")
            TEXT("兵　兵　兵　兵　兵")
            TEXT("　　　　　　　　　")
            TEXT("　　　　　　　　　")
            TEXT("卒　卒　卒　卒　卒")
            TEXT("　炮　　　　　炮　")
            TEXT("　　　　　　　　　")
            TEXT("車馬象士將士象馬車");
        break;
    }
    /*
        FIXME: 黑子模式需要改動的東西太多了
    */
    map = 
            TEXT("車馬象士將士象馬車")
            TEXT("　　　　　　　　　")
            TEXT("　炮　　　　　炮　")
            TEXT("卒　卒　卒　卒　卒")
            TEXT("　　　　　　　　　")
            TEXT("　　　　　　　　　")
            TEXT("兵　兵　兵　兵　兵")
            TEXT("　砲　　　　　砲　")
            TEXT("　　　　　　　　　")
            TEXT("俥傌相仕帥仕相傌俥");
    /*
        釋放所有的棋子
    */
    while((&chesshead)->next){
        CHESSNODE * chessnode = (&chesshead)->next;
        (&chesshead)->next = chessnode->next;
        free(chessnode);
    }
    /*
        重置 turn
    */
    turn = 0;

    for(i=0; map[i]!=0; i++){
        switch(map[i]){
        case TEXT('車'):
        case TEXT('馬'):
        case TEXT('象'):
        case TEXT('士'):
        case TEXT('將'):
        case TEXT('炮'):
        case TEXT('卒'):
            CreateChess(i%9,i/9,BLACK,map[i]);
            break;
        case TEXT('俥'):
        case TEXT('傌'):
        case TEXT('相'):
        case TEXT('仕'):
        case TEXT('帥'):
        case TEXT('砲'):
        case TEXT('兵'):
            CreateChess(i%9,i/9,RED,map[i]);
            break;
        }

    }
    return 0;
}

int DrawWay(HDC mdc,CHESSNODE * const oneway){
    RECT rt = {0,0,CHESS_SIZE,CHESS_SIZE};
    int padding = 10;
    HBRUSH brush = CreateSolidBrush(RGB(0,243,150)),oldbrush;
    oldbrush = (HBRUSH)SelectObject(mdc,brush);

    OffsetRect(&rt,oneway->chess.x*CHESS_SIZE,CHAR_SIZE+oneway->chess.y*CHESS_SIZE);
    Ellipse(mdc,rt.left+padding,rt.top+padding,rt.right-padding,rt.bottom-padding);
    SelectObject(mdc,oldbrush);
    DeleteObject(brush);
    return 0;
}

int DrawValidWay(HDC mdc){
    CHESSNODE * oneway = &chessway;
    while(oneway->next){
        oneway = oneway->next;
        DrawWay(mdc,oneway);
    }
    return 0;
}

CHESSNODE * chessdrag;
int Render(){
    CHESSNODE * chessnodefather = &chesshead;
    HDC hdc = GetDC(hWnd);
    
    /*
        複製棋盤的 dc
    */
    BitBlt(mdc,0,0,WIDTH,HEIGHT,bgdc,0,0,SRCCOPY);
    /*
        繪製在棋盤上的所有棋子
    */
    while(chessnodefather->next){
        chessnodefather = chessnodefather->next;
        DrawChess(mdc,&chessnodefather->chess);
    }
    /*
        繪製被選中的棋子能走的路徑
    */
    DrawValidWay(mdc);
    /*
        繪製被選中的棋子
    */
    if(chessdrag){
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(hWnd,&pt);
        DrawChessXY(mdc,&chessdrag->chess,pt.x-CHESS_SIZE/2,pt.y-CHESS_SIZE/2);
    }
    /*
        繪製
    */
    BitBlt(hdc,0,0,WIDTH,HEIGHT,mdc,0,0,SRCCOPY);
    ReleaseDC(hWnd,hdc);
    ValidateRect(hWnd,NULL);
    return 0;
}

CHESSNODE * HitChess(int x,int y){
    /*
        返回被擊中的棋子的指針，並將之從鏈表中刪除
    */
    CHESSNODE * chessnodefather = &chesshead;
    CHESSNODE * chessnode = &chesshead;
    while(chessnode->next){
        chessnodefather = chessnode;
        chessnode = chessnode->next;
        if(chessnode->chess.x == x && chessnode->chess.y == y){
            chessnodefather->next = chessnode->next;
            return chessnode;
        }
    }
    return NULL;
}

#define InField(X,Y)    ((X)>=0 && (X)<9 && (Y)>=0 && (Y)<10)
int IsEmpty(int x,int y){
    CHESSNODE * chessnode = &chesshead;
    while(chessnode->next){
        chessnode = chessnode->next;
        if(chessnode->chess.x == x && chessnode->chess.y == y){
            return 0;
        }
    }
    return 1;
}
int IsAlly(int x,int y,int owner){
    CHESSNODE * chessnode = &chesshead;
    while(chessnode->next){
        chessnode = chessnode->next;
        if(chessnode->chess.x == x && chessnode->chess.y == y && chessnode->chess.owner == owner){
            return 1;
        }
    }
    return 0;
}
int IsEnemy(int x,int y,int owner){
    CHESSNODE * chessnode = &chesshead;
    while(chessnode->next){
        chessnode = chessnode->next;
        if(chessnode->chess.x == x && chessnode->chess.y == y && chessnode->chess.owner != owner){
            return 1;
        }
    }
    return 0;
}
int InSide(int y,int owner){
    switch(owner)
    {
    case BLACK:
        return y <= 4;
    case RED:
        return y >= 5;
    }
}

int InPalace(int x,int y,int owner){
    int map[][2] = {
        /* BLACK */
        3,0,    4,0,    5,0,
        3,1,    4,1,    5,1,
        3,2,    4,2,    5,2,
        /* RED */
        3,7,    4,7,    5,7,
        3,8,    4,8,    5,8,
        3,9,    4,9,    5,9,
    };
    int i;

    switch(owner){
    case BLACK:
        for(i = 0;i < ARRAYSIZE(map)/2; i++){
            if(x == map[i][0] && y == map[i][1]){
                return 1;
            }
        }
        return 0;
    case RED:
        for(i = ARRAYSIZE(map)/2;i < ARRAYSIZE(map); i++){
            if(x == map[i][0] && y == map[i][1]){
                return 1;
            }
        }
        return 0;
    }
}

int PushWay(int x,int y){
    CHESSNODE * onewayfather = &chessway;
    CHESSNODE * oneway = (CHESSNODE *)malloc(sizeof(CHESSNODE));

    oneway->chess.x = x;
    oneway->chess.y = y;
    oneway->next    = onewayfather->next;
    onewayfather->next = oneway;
    return 0;
}

TCHAR GetChessName(int x,int y){
    CHESSNODE * chessnode = &chesshead;
    while(chessnode->next){
        chessnode = chessnode->next;
        if(chessnode->chess.x == x && chessnode->chess.y == y){
            return chessnode->chess.text[0];
        }
    }
    return NULL;
}

static TCHAR text[2048] = {0};
static int index = 0;

int UpdateChessMoveText(CHESS *chess,int tx,int ty){
    /*
        這個函數負責更新 text 的內容
    */
    TCHAR ch = TEXT('走');
    TCHAR num[2][10] = {TEXT("九八七六五四三二一"),TEXT("一二三四五六七八九")};
    CHESSNODE * chessnodefather = &chesshead;
    int i = 0;

    /*
        特殊情况
    */
    while(chessnodefather->next){
        chessnodefather = chessnodefather->next;
        if(chessnodefather->chess.text[0] == chess->text[0] && 
           &(chessnodefather->chess)!=chess                 &&
           chessnodefather->chess.x == chess->x
           ){
            i = 1;
            switch(chess->owner){
            case BLACK:
                if(chess->y > chessnodefather->chess.y){
                    ch = TEXT('前');
                }else{
                    ch = TEXT('後');
                }
                break;
            case RED:
                if(chess->y < chessnodefather->chess.y){
                    ch = TEXT('前');
                }else{
                    ch = TEXT('後');
                }
                break;
            }
            index += wsprintf(text+index,TEXT("%c%s"),ch,chess->text);

            if(chess->text[0] == TEXT('兵') || chess->text[0] == TEXT('卒')){
                index += wsprintf(text+index,TEXT("%c"),num[chess->owner][chess->x]);
            }

            break;
        }
    }

    if(!i){
        index += wsprintf(text+index,TEXT("%s%c"),chess->text,num[chess->owner][chess->x]);
    }

    if(chess->owner == BLACK){
        if(ty > chess->y)   ch = TEXT('進');
        if(ty == chess->y)  ch = TEXT('平');
        if(ty < chess->y)   ch = TEXT('退');
    }
    if(chess->owner == RED){
        if(ty < chess->y)   ch = TEXT('進');
        if(ty == chess->y)  ch = TEXT('平');
        if(ty > chess->y)   ch = TEXT('退');
    }
    /*
        “马”、“象”、“士”三个兵种因为不行直线所以没有“平”，它们的进退数字含义是从第几条纵线“走”到了第几条纵线。
        （这点如同走直线棋子的“平”）。 

        对于走直线兵种“车、炮、兵（卒）、帅（将）”
        而进退时，最后一个数字的含义是经过了多少个小格。
    */
    index += wsprintf(text+index,TEXT("%c"),ch);
    switch(ch){
    case TEXT('進'):
    case TEXT('退'):
        switch(chess->text[0]){
        case TEXT('車'):
        case TEXT('俥'):
        case TEXT('砲'):
        case TEXT('炮'):
        case TEXT('卒'):
        case TEXT('兵'):
        case TEXT('將'):
        case TEXT('帥'):
            i = chess->y;
            i = (i-ty > 0) ? i-ty : ty-i;
            index += wsprintf(text+index,TEXT("%c\r\n"),num[1][i-1]);
            return 0;
        }
    default:
        index += wsprintf(text+index,TEXT("%c\r\n"),num[chess->owner][tx]);
    }
    return 0;
}

int KeyDown(){
    POINT pt;
    int x,y,i,f;

    if(GetAsyncKeyState(VK_LBUTTON)){
        /*
            獲取選中的棋子    
        */
        if(chessdrag==NULL){
            GetCursorPos(&pt);
            ScreenToClient(hWnd,&pt);
            x = pt.x/CHESS_SIZE;
            y = (pt.y-CHAR_SIZE)/CHESS_SIZE;
            chessdrag = HitChess(x,y);
            if(chessdrag){
                /*
                    檢測是否是當前的回合
                */
                if(chessdrag->chess.owner != turn%2){
                    CHESSNODE * chessnodefather = &chesshead;
                    chessdrag->next = chessnodefather->next;
                    chessnodefather->next = chessdrag;
                    chessdrag = NULL;
                    return -1;
                }
                
                printf("(%d,%d,%c)\n",x,y,chessdrag->chess.owner==BLACK?'b':'r');
                pt.x = x*CHESS_SIZE+CHESS_SIZE/2;
                pt.y = y*CHESS_SIZE+CHESS_SIZE/2;
                ClientToScreen(hWnd,&pt);
                SetCursorPos(pt.x,pt.y+CHAR_SIZE);
                /*
                    更新棋子能走的路徑
                */
                int i,j;
                CHESSNODE * onewayfather = &chessway;
                CHESSNODE * oneway = (CHESSNODE *)malloc(sizeof(CHESSNODE));

                switch(chessdrag->chess.text[0]){
                case TEXT('車'):
                case TEXT('俥'):
                    for(i=1; i<10; i++){
                        if(InField(x+i,y)){
                            if(IsAlly(x+i,y,chessdrag->chess.owner))break;
                            if(IsEnemy(x+i,y,chessdrag->chess.owner)){
                                PushWay(x+i,y);
                                break;
                            }
                            PushWay(x+i,y);
                        }else break;
                    }
                    for(i=1; i<10; i++){
                        if(InField(x-i,y)){
                            if(IsAlly(x-i,y,chessdrag->chess.owner))break;
                            if(IsEnemy(x-i,y,chessdrag->chess.owner)){
                                PushWay(x-i,y);
                                break;
                            }
                            PushWay(x-i,y);
                        }else break;
                    }
                    for(i=1; i<10; i++){
                        if(InField(x,y+i)){
                            if(IsAlly(x,y+i,chessdrag->chess.owner))break;
                            if(IsEnemy(x,y+i,chessdrag->chess.owner)){
                                PushWay(x,y+i);
                                break;
                            }
                            PushWay(x,y+i);
                        }else break;
                    }
                    for(i=1; i<10; i++){
                        if(InField(x,y-i)){
                            if(IsAlly(x,y-i,chessdrag->chess.owner))break;
                            if(IsEnemy(x,y-i,chessdrag->chess.owner)){
                                PushWay(x,y-i);
                                break;
                            }
                            PushWay(x,y-i);
                        }else break;
                    }
                    break;
                case TEXT('炮'):
                case TEXT('砲'):
                    f = 0;
                    for(i=1; i<10; i++){
                        if(InField(x+i,y)){
                            if(!f){
                                if(IsEmpty(x+i,y)){
                                    PushWay(x+i,y);
                                }else{
                                    f = 1;
                                }
                                continue;
                            }
                            if(f){
                                if(IsEnemy(x+i,y,chessdrag->chess.owner)){
                                    PushWay(x+i,y);
                                    break;
                                }
                            }
                        }else break;
                    }
                    f = 0;
                    for(i=1; i<10; i++){
                        if(InField(x-i,y)){
                            if(!f){
                                if(IsEmpty(x-i,y)){
                                    PushWay(x-i,y);
                                }else{
                                    f = 1;
                                }
                                continue;
                            }
                            if(f){
                                if(IsEnemy(x-i,y,chessdrag->chess.owner)){
                                    PushWay(x-i,y);
                                    break;
                                }
                            }
                        }else break;
                    }
                    f = 0;
                    for(i=1; i<10; i++){
                        if(InField(x,y+i)){
                            if(!f){
                                if(IsEmpty(x,y+i)){
                                    PushWay(x,y+i);
                                }else{
                                    f = 1;
                                }
                                continue;
                            }
                            if(f){
                                if(IsEnemy(x,y+i,chessdrag->chess.owner)){
                                    PushWay(x,y+i);
                                    break;
                                }
                            }
                        }else break;
                    }
                    f = 0;
                    for(i=1; i<10; i++){
                        if(InField(x,y-i)){
                            if(!f){
                                if(IsEmpty(x,y-i)){
                                    PushWay(x,y-i);
                                }else{
                                    f = 1;
                                }
                                continue;
                            }
                            if(f){
                                if(IsEnemy(x,y-i,chessdrag->chess.owner)){
                                    PushWay(x,y-i);
                                    break;
                                }
                            }
                        }else break;
                    }
                    break;
                case TEXT('馬'):
                case TEXT('傌'):
                    /* 左 */
                    if(InField(x-2,y+1) && IsEmpty(x-1,y) && !IsAlly(x-2,y+1,chessdrag->chess.owner)){
                        PushWay(x-2,y+1);
                    }
                    if(InField(x-2,y-1) && IsEmpty(x-1,y) && !IsAlly(x-2,y-1,chessdrag->chess.owner)){
                        PushWay(x-2,y-1);
                    }
                    /* 上 */
                    if(InField(x-1,y-2) && IsEmpty(x,y-1) && !IsAlly(x-1,y-2,chessdrag->chess.owner)){
                        PushWay(x-1,y-2);
                    }
                    if(InField(x+1,y-2) && IsEmpty(x,y-1) && !IsAlly(x+1,y-2,chessdrag->chess.owner)){
                        PushWay(x+1,y-2);
                    }
                    /* 右 */
                    if(InField(x+2,y-1) && IsEmpty(x+1,y) && !IsAlly(x+2,y-1,chessdrag->chess.owner)){
                        PushWay(x+2,y-1);
                    }
                    if(InField(x+2,y+1) && IsEmpty(x+1,y) && !IsAlly(x+2,y+1,chessdrag->chess.owner)){
                        PushWay(x+2,y+1);
                    }
                    /* 下 */
                    if(InField(x+1,y+2) && IsEmpty(x,y+1) && !IsAlly(x+1,y+2,chessdrag->chess.owner)){
                        PushWay(x+1,y+2);
                    }
                    if(InField(x-1,y+2) && IsEmpty(x,y+1) && !IsAlly(x-1,y+2,chessdrag->chess.owner)){
                        PushWay(x-1,y+2);
                    }
                    break;
                case TEXT('象'):
                case TEXT('相'):
                    if(InField(x-2,y-2) && IsEmpty(x-1,y-1) && InSide(y-2,chessdrag->chess.owner) && !IsAlly(x-2,y-2,chessdrag->chess.owner)){
                        PushWay(x-2,y-2);
                    }
                    if(InField(x+2,y-2) && IsEmpty(x+1,y-1) && InSide(y-2,chessdrag->chess.owner) && !IsAlly(x+2,y-2,chessdrag->chess.owner)){
                        PushWay(x+2,y-2);
                    }
                    if(InField(x+2,y+2) && IsEmpty(x+1,y+1) && InSide(y+2,chessdrag->chess.owner) && !IsAlly(x+2,y+2,chessdrag->chess.owner)){
                        PushWay(x+2,y+2);
                    }
                    if(InField(x-2,y+2) && IsEmpty(x-1,y+1) && InSide(y+2,chessdrag->chess.owner) && !IsAlly(x-2,y+2,chessdrag->chess.owner)){
                        PushWay(x-2,y+2);
                    }
                    break;
                case TEXT('兵'):
                    if(InSide(y,RED)){
                        if(!IsAlly(x,y-1,RED)){
                            PushWay(x,y-1);
                        }
                    }else{
                        if(InField(x,y-1) && !IsAlly(x,y-1,RED)){
                            PushWay(x,y-1);
                        }
                        if(InField(x-1,y) && !IsAlly(x-1,y,RED)){
                            PushWay(x-1,y);
                        }
                        if(InField(x+1,y) && !IsAlly(x+1,y,RED)){
                            PushWay(x+1,y);
                        }
                    }
                    break;
                case TEXT('卒'):
                    if(InSide(y,BLACK)){
                        if(!IsAlly(x,y+1,BLACK)){
                            PushWay(x,y+1);
                        }
                    }
                    else{
                        if(InField(x,y+1) && !IsAlly(x,y+1,BLACK)){
                            PushWay(x,y+1);
                        }
                        if(InField(x-1,y) && !IsAlly(x-1,y,BLACK)){
                            PushWay(x-1,y);
                        }
                        if(InField(x+1,y) && !IsAlly(x+1,y,BLACK)){
                            PushWay(x+1,y);
                        }
                    }
                    break;
                case TEXT('士'):
                    if(InPalace(x+1,y+1,BLACK) && !IsAlly(x+1,y+1,BLACK)){
                        PushWay(x+1,y+1);
                    }
                    if(InPalace(x-1,y+1,BLACK) && !IsAlly(x-1,y+1,BLACK)){
                        PushWay(x-1,y+1);
                    }
                    if(InPalace(x+1,y-1,BLACK) && !IsAlly(x+1,y-1,BLACK)){
                        PushWay(x+1,y-1);
                    }
                    if(InPalace(x-1,y-1,BLACK) && !IsAlly(x-1,y-1,BLACK)){
                        PushWay(x-1,y-1);
                    }
                    break;
                case TEXT('仕'):
                    if(InPalace(x+1,y+1,RED) && !IsAlly(x+1,y+1,RED)){
                        PushWay(x+1,y+1);
                    }
                    if(InPalace(x-1,y+1,RED) && !IsAlly(x-1,y+1,RED)){
                        PushWay(x-1,y+1);
                    }
                    if(InPalace(x+1,y-1,RED) && !IsAlly(x+1,y-1,RED)){
                        PushWay(x+1,y-1);
                    }
                    if(InPalace(x-1,y-1,RED) && !IsAlly(x-1,y-1,RED)){
                        PushWay(x-1,y-1);
                    }
                    break;
                case TEXT('帥'):
                case TEXT('將'):
                    if(InPalace(x+1,y,chessdrag->chess.owner) && !IsAlly(x+1,y,chessdrag->chess.owner)){
                        PushWay(x+1,y);
                    }
                    if(InPalace(x-1,y,chessdrag->chess.owner) && !IsAlly(x-1,y,chessdrag->chess.owner)){
                        PushWay(x-1,y);
                    }
                    if(InPalace(x,y+1,chessdrag->chess.owner) && !IsAlly(x,y+1,chessdrag->chess.owner)){
                        PushWay(x,y+1);
                    }
                    if(InPalace(x,y-1,chessdrag->chess.owner) && !IsAlly(x,y-1,chessdrag->chess.owner)){
                        PushWay(x,y-1);
                    }
                    /* 飛宮 */
                    for(i=1; i<10; i++){
                        if(InField(x,y+i)){
                            if(IsAlly(x,y+i,chessdrag->chess.owner))break;
                            if(IsEnemy(x,y+i,chessdrag->chess.owner)){
                                TCHAR c = GetChessName(x,y+i);
                                if(c == TEXT('帥') || c == TEXT('將')){
                                    PushWay(x,y+i);
                                }
                                break;
                            }
                        }
                        else break;
                    }
                    for(i=1; i<10; i++){
                        if(InField(x,y-i)){
                            if(IsAlly(x,y-i,chessdrag->chess.owner))break;
                            if(IsEnemy(x,y-i,chessdrag->chess.owner)){
                                TCHAR c = GetChessName(x,y-i);
                                if(c == TEXT('帥') || c == TEXT('將')){
                                    PushWay(x,y-i);
                                }
                                break;
                            }
                        }
                        else break;
                    }
                    break;
                }
            }
        }
    }else if(chessdrag){
        /*
            放下選中的棋子，如果該位置不是這個棋子可以走的位置，則把棋子放回原處
        */
        GetCursorPos(&pt);
        ScreenToClient(hWnd,&pt);
        x = pt.x/CHESS_SIZE;
        y = (pt.y-CHAR_SIZE)/CHESS_SIZE;
        /*
            判斷移動是否合法
        */
        CHESSNODE * oneway = &chessway;
        int f = 0;
        while(oneway->next){
            oneway = oneway->next;
            if(oneway->chess.x == x && oneway->chess.y == y){
                f = 1;
                break;
            }
        }
        if(f){
            int iStart,iEnd;
            /*
                給 edit 控件發送消息
            */
            UpdateChessMoveText(&chessdrag->chess,x,y); 
            SetWindowText(hText,text);
            SendMessage(hText,WM_SETFOCUS,0,0);
            SendMessage(hText,EM_SCROLL,SB_BOTTOM,0);
            /*
                能夠進行移動，進入下一個回合
            */
            turn ++;
            chessdrag->chess.x = x;
            chessdrag->chess.y = y;
            /*
                檢測XY格子是否有棋子，有則將之移除
            */
            CHESSNODE * chessnode;
            if(chessnode = HitChess(x,y)){
                if(chessnode){
                    if(chessnode->chess.text[0] == TEXT('帥') || chessnode->chess.text[0] == TEXT('將')){
                        MessageBox(hWnd,TEXT("遊戲結束"),TEXT("中國象棋"),MB_OK);
                    }
                    free(chessnode);
                }
            }
        }
        /*
            將被選中的棋子加入鏈表
        */
        CHESSNODE * chessnodefather = &chesshead;
        chessdrag->next = chessnodefather->next;
        chessnodefather->next = chessdrag;
        /*
            清空所有路徑
        */
        CHESSNODE * onewayfather = chessway.next;
        while(onewayfather){
            CHESSNODE * oneway = onewayfather;
            onewayfather = onewayfather->next;
            free(oneway);
        }
        chessway.next = NULL;
        chessdrag = NULL;
    }
}

int ClearDC(){
    RECT rt = {0,0,WIDTH,HEIGHT};
    FillRect(bgdc,&rt,(HBRUSH)GetStockObject(WHITE_BRUSH));
    FillRect(mdc,&rt,(HBRUSH)GetStockObject(WHITE_BRUSH));
    DrawChessField(bgdc);
    return 0;
}

int OnCreate(){
    HDC hdc = GetDC(hWnd);
    HBITMAP map = CreateCompatibleBitmap(hdc,WIDTH,HEIGHT);
    HBITMAP bgmap = CreateCompatibleBitmap(hdc,WIDTH,HEIGHT);

    mdc = CreateCompatibleDC(hdc);
    SelectObject(mdc,map);
    bgdc = CreateCompatibleDC(hdc);
    SelectObject(bgdc,bgmap);

    ClearDC();
    InitChess(RED);

    fontlisu = CreateFont(FONT_SIZE,0,0,0,0,0,0,0,GB2312_CHARSET,0,0,0,FIXED_PITCH,TEXT("LiSu"));

    ReleaseDC(hWnd,hdc);
    return 0;
}

long __stdcall WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam){
    static int  cxClient,cyClient;
    PAINTSTRUCT ps;

    switch(message){
    case WM_CREATE:
        OnCreate();
        hText = CreateWindow(TEXT("edit"),NULL,WS_CHILD|WS_VISIBLE|WS_VSCROLL|ES_RIGHT|ES_MULTILINE|
                             ES_AUTOVSCROLL|ES_AUTOHSCROLL|ES_NOHIDESEL,
                             WIDTH,0,TEXT_WIDTH,HEIGHT,hWnd,(HMENU)ID_EDIT,((LPCREATESTRUCT)lParam)->hInstance,NULL);
        {
            HFONT hFont = CreateFont(28,0,0,0,0,0,0,0,GB2312_CHARSET,0,0,0,FIXED_PITCH,TEXT("LiSu"));
            SendMessage(hText,WM_SETFONT,(WPARAM)hFont,(LPARAM)TRUE);
        }
        return 0;
    case WM_SIZE:
        cxClient = LOWORD(lParam);
        cyClient = HIWORD(lParam);
        return 0;
    case WM_COMMAND:
        switch(LOWORD(wParam)){
        case IDM_EXIT:
            DestroyWindow(hWnd);
            return 0;
        case IDM_RED:
        case IDM_BLACK:
            ClearDC();
            InitChess(LOWORD(wParam)-IDM_RED);
            return 0;
        case IDM_DEBUG:
            AllocConsole();
            freopen("CONOUT$","w",stdout);
            freopen("CONIN$","r",stdin);
            return 0;
        case IDM_NODEBUG:
            printf("free\n");
            FreeConsole();
            return 0;
        default:
            break;
        }
    case WM_PAINT:
        BeginPaint(hWnd,&ps);
        EndPaint(hWnd,&ps);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd,message,wParam,lParam);
}

int __stdcall WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR szCmdLine,int nCmdShow){
    int x,y,w = WIDTH+TEXT_WIDTH,h = HEIGHT;
    RECT rt = {0,0,w,h};
    WNDCLASSEX wcex = {0};
    MSG msg = {0};
    HMENU hMenu,hMenuPopup;
    
    hMenu = CreateMenu();
    hMenuPopup = CreateMenu();
    AppendMenu(hMenuPopup,MF_STRING,IDM_RED,    TEXT("重新開始"));
    //AppendMenu(hMenuPopup,MF_STRING,IDM_BLACK,  TEXT("黑子"));
    AppendMenu(hMenuPopup,MF_STRING,IDM_EXIT,   TEXT("退出(&x)"));
    AppendMenu(hMenu,MF_POPUP,(UINT_PTR)hMenuPopup,   TEXT("遊戲"));
#ifdef DEBUG
    AppendMenu(hMenu,MF_POPUP,IDM_DEBUG,    TEXT("&Debug"));
    AppendMenu(hMenu,MF_POPUP,IDM_NODEBUG,  TEXT("&No debug"));
#endif
    wcex.cbSize         = sizeof(wcex);
    wcex.hCursor        = LoadCursor(0,IDC_ARROW);
    wcex.hInstance      = hInstance;
    wcex.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wcex.lpfnWndProc    = WndProc;
    wcex.lpszClassName  = TEXT("WndClass");
    RegisterClassEx(&wcex);

    AdjustWindowRectEx(&rt,WS_OVERLAPPEDWINDOW,TRUE,NULL);
    w = rt.right-rt.left;
    h = rt.bottom-rt.top;
    SystemParametersInfo(SPI_GETWORKAREA,NULL,&rt,NULL);
    x = rt.left + (rt.right - rt.left - w) / 2;
    y = rt.top  + (rt.bottom - rt.top - h) / 2;
    
    hWnd = CreateWindow(TEXT("WndClass"),TEXT("中國象棋 ver 0.2"),WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME &~WS_MAXIMIZEBOX,
                            x,y,w,h,0,hMenu,hInstance,0);

    ShowWindow(hWnd,nCmdShow);
    while(msg.message != WM_QUIT){
        if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }else{
            KeyDown();
            Render();
            Sleep(40);
        }
    }
    return msg.wParam;
}