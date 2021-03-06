#include <stdio.h>
#include <windows.h>
#include <assert.h>

#define EXENAME     L"MdBlogToHtml"
#define VERSION     L"0.38"

#define SET_CONSOLE_COLOR(c)    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),c)


/*
# 說明
    這個程序會將一個MD各式的文件轉為一個HTML格式的文件

# 用法
    程序名 MD格式的文件名

# 支持的格式
    #   H1~H6   - +     UL
    \\t PRE     <http   A
    `   CODE    []()    A
    **  B       ![]()   IMG
    *   I
    >   BLOCKQUOTE

# 額外規則
    <DIV></DIV>之間的東西不進行編譯
    `;`為單獨一行時編譯為<P>&nbsp;</P>

# 以後考慮加入的功能
    HR  [^n]    ``` ==  OL __ 1. a.

# 注意：
    文檔必須以 Unicode 格式(UTF16-LE+BOM)存儲
    輸出的文檔格式同樣為 Unicode
    換行各式為 CR+LF ("\r\n"  HEX := 0x000D,0x000A)
    目前版本沒有語法查錯機制，語法解析錯誤會導致程序崩潰

# BUGs:
    文檔請在最後末尾留下幾個空行，以防止最後一行不能被解析
    我嘗試著修復這個BUG，但是失敗了，所以，每個文檔請在末尾留兩個空白行
    在寫入文件時，我沒有檢測是否寫入成功，請保持硬盤空間充足

frankrx41   2019-04-11
*/

/*
    buf1 ~ 3
    在識別文檔前提前寫入
*/
wchar_t buf1[] = 
L"<!DOCTYPE HTML>\r\n"
L"<HTML>\r\n"
L"<HEAD>\r\n"
//L"<TITLE></TITLE>"
//L"<META charset=\"UTF-16\" />\r\n"
L"<META http-equiv=\"Content-Type\" content=\"text/html; charset=utf-16\" />\r\n"
L"<META name=\"viewport\" content=\"width=device-width,initial-scale=1,"
L"maximum-scale=1,minimum-scale=1,user-scalable=yes\" />\r\n"
L"<SCRIPT src=\"script.js\"></SCRIPT>\r\n"
L"<LINK href=\"style.css\" rel=\"stylesheet\" type=\"text/css\" />\r\n"
L"</HEAD>\r\n\r\n"
;

wchar_t buf2[] = 
L"<BODY>\r\n"
//L"<ARTICLE>\r\n"
;

wchar_t buf3[] = 
//L"</ARTICLE>\r\n"
L"\r\n"
//L"<FOOTER>frankrx41.github.io 2019</FOOTER>\r\n"
L"</BODY>\r\n"
L"<!-- "EXENAME L" ver "VERSION L" -->\r\n"
L"</HTML>\r\n"
;

/*
    解析一行
    會根據上一次的解析結果繼續解析
    當全文解析完成後請再一次呼叫本函式，並將 szBuf1 置為 NULL 以吐出所有的閉標籤

    szBuf1 變數中的內容會被修改
    szBuf2 保存返回的內容
*/
int AnalyzeOneLine(wchar_t /* NOT CONST */ * IN szBuf1,wchar_t * OUT szBuf2){
    int i,j;    // 因為C99中不能夠再使用for修改外部變數的特性
    
    int         tag     = 0;            // 默認為[p]
/*
    `tag`使用
    [br][blockquote]用自己獨享的標誌

    上下文無關   P H1~H6 LI
    上下文相關   PRE UL OL
*/
#define UNKONW  ( 0x40 )

#define P       ( 0x00 )
#define H1      ( 0x01 )
#define H2      ( 0x02 )
#define H3      ( 0x03 )
#define H4      ( 0x04 )
#define H5      ( 0x05 )
#define H6      ( 0x06 )
#define PRE     ( 0x08 )
#define LI      ( 0x09 )

// 以下標籤會跳過編譯
#define SCRIPT  ( 0x80 )
#define TABLE   ( 0x81 )
#define DIV     ( 0x82 )
#define STYLE   ( 0x83 )
#define P_EX    ( 0x87 )

#define DO_NOT_COMPILER_TAG(x)  ((x)&0x80)

    static int  lasttag = UNKONW;

    BOOL        br      = 0;

    int         blockquote      = 0;
    static int  lastblockquote  = 0;

    static int  haveblankline   = 0;


#define UL      ( 0x10 )
#define OL      ( 0x11 )
    static int  list[8];    // 其內部取值只能為 UL 或 OL
    static int  lastlistindex;
    int         listindex = 0;


    int s1 = 0;     // szBuf1 的指針
    int s2 = 0;     // szBuf2 的指針
    int x;

    if(szBuf1 != NULL && !DO_NOT_COMPILER_TAG(lasttag)){
        
        // `szBuf1`的長度，本過程將會修改之
        int l = wcslen(szBuf1);
        
        // 移除行尾{空白}和{TAB}，當移除的{空白}和{TAB}大於2時，設置[BR]標誌
        for(i=l-1; szBuf1[i] == L' ' || szBuf1[i] == L'\t'; i--)/*szBuf1[i]=0*/;
        if(l-i > 2)br = 1;

        // 重新設置`szBuf1`的長度，並且忽略空白行
        l = i+1;
        if(l == 0){
            haveblankline = 1;
            return szBuf2[0] = 0;
        }
        szBuf1[i+1] = 0;

        // 額外規則
        // `;`為一行的視為空行
        if(wcscmp(szBuf1,L";") == 0){
            //swprintf(szBuf2+s2,L"<P>&nbsp;<P>\r\n");
            haveblankline = 1;
            tag = P_EX;
            //return 0;
        }

        // [Pre]
        // 當行首存在4個空白或一個{TAB}時
        int iscode = 0;
        for(i=0; !iscode && szBuf1[s1+i] != 0 && (szBuf1[s1+i] == L' '||szBuf1[s1+i] == L'\t'); i++)
            if(szBuf1[s1+i] == L'\t' || i == 3)iscode = 1;
        if(iscode){
            tag = PRE;
        }
        s1 += i;

        // [BlockQuote]
        // 有多少個`>`即代表有多少級[BlockQuote]
        for(; tag == P && szBuf1[s1] == L'>'; s1++){
            blockquote++;
        }

        // [UL]
        // 以`-`或`+`打頭
        // FIXME: 增加對 OL 的支援
        for(; tag == P && (szBuf1[s1] == L'-' || szBuf1[s1] == L'+' || szBuf1[s1] == L' '); s1+=1){
            if(szBuf1[s1] == L'-' || szBuf1[s1] == L'+'){
                list[listindex++] = UL;
            }
        }
        if(listindex > 0){
            tag = LI;
        }

        // [Title]
        for(i=0; tag == P && szBuf1[s1+i] == L'#'; i++);
        if(i != 0){
            // 設置[Hn]標誌，其中 H1~H6 == 1~0
            tag = i;
            // 忽略末尾的空白和`#`
            // `## XXX ##`這樣的情況也要清除掉末尾`#`前的空白，所以需要檢測空白
            for(j=l-1; szBuf1[j] == L' ' || szBuf1[j] == L'#'; j--);
            szBuf1[j+1] = 0;
            // 清除掉開頭`#`後面的空白
            for(s1=s1+i;szBuf1[s1] == L' ';s1++);
        }

        if(wcscmp(szBuf1+s1,L"<DIV>") == 0){
            tag = DIV;
        }

        if(wcscmp(szBuf1+s1,L"<SCRIPT>") == 0){
            tag = SCRIPT;
        }

        if(wcscmp(szBuf1+s1,L"<TABLE>") == 0){
            tag = TABLE;
        }

        if(wcscmp(szBuf1+s1,L"<STYLE>") == 0){
            tag = STYLE;
        }

        if(tag != P){
            // 改寫[br]標誌
            br = 0;
        }
    }else{
        if(szBuf1 == NULL){
            tag = UNKONW;
        }else{
            tag = lasttag;
        }
    }

    if(!DO_NOT_COMPILER_TAG(lasttag) || szBuf1 == NULL){
        // [blockquote]
        // 保持上一次的[blockquote]狀態
        if(tag == P && !haveblankline && blockquote == 0 && lastblockquote > 0 && szBuf1 != NULL){
            blockquote = lastblockquote;
        }

        // [P]
        if( lasttag == P && ( tag != P || haveblankline || szBuf1 == NULL || lastlistindex != listindex || lastblockquote != blockquote) )
        {
            lasttag = UNKONW;
            s2 += swprintf(szBuf2+s2,L"</P>\r\n");
        }

        // [UL]
        if(lastlistindex > listindex){
            for(i=0; i< lastlistindex - listindex;i++)
                s2 += swprintf(szBuf2+s2,L"</UL>\r\n");
        }

        // [pre]
        if(lasttag == PRE && tag != PRE){
            s2 += swprintf(szBuf2+s2,L"</CODE></PRE>\r\n");
        }


        // [blockquote]
        if(lastblockquote < blockquote){
            for(i=0; i< blockquote - lastblockquote;i++)
                s2 += swprintf(szBuf2+s2,L"<BLOCKQUOTE>\r\n");
        }

        // [pre]
        if(lasttag != PRE && tag == PRE){
            s2 += swprintf(szBuf2+s2,L"<PRE><CODE>");
            haveblankline = 0;
        }

        // [ul]
        if(lastlistindex < listindex){
            for(i=0; i< listindex - lastlistindex;i++)
                s2 += swprintf(szBuf2+s2,L"<UL>\r\n");
        }

        // [blockquote]
        // 因為[blockquote]優先級關係，必須將閉標籤的吐出放在[pre]和[ul]之後
        if(lastblockquote > blockquote){
            for(i=0; i< lastblockquote - blockquote;i++)
                s2 += swprintf(szBuf2+s2,L"</BLOCKQUOTE>\r\n");
        }

        // [P]
        if(lasttag != P && tag == P && szBuf1 != NULL){
            s2 += swprintf(szBuf2+s2,L"<P>");
        }
    }

    if(szBuf1 == NULL){
        szBuf2[s2] = 0;
        SET_CONSOLE_COLOR(0xCF);
        return 1;
    }

    // 上下文無關標籤處理
    switch(tag){
        /*
    case P:
        s2 += swprintf(szBuf2+s2,L"<P>");
        break;
        */
    case H1:
        s2 += swprintf(szBuf2+s2,L"<H1>");
        break;
    case H2:
        s2 += swprintf(szBuf2+s2,L"<H2>");
        break;
    case H3:
        s2 += swprintf(szBuf2+s2,L"<H3>");
        break;
    case H4:
        s2 += swprintf(szBuf2+s2,L"<H4>");
        break;
    case H5:
        s2 += swprintf(szBuf2+s2,L"<H5>");
        break;
    case H6:
        s2 += swprintf(szBuf2+s2,L"<H6>");
        break;
    case LI:
        s2 += swprintf(szBuf2+s2,L"<LI>");
        break;
    }

    // Normal:
    if(tag != PRE && !DO_NOT_COMPILER_TAG(tag)){
    for(;szBuf1[s1] != 0;){
        if(szBuf1[s1] == L'*'){
            for(i = 0;szBuf1[s1+i] == L'*';i++);
            // [Italic]
            if(i == 1){
                s1++;
                for(j=s1+1;szBuf1[j] != L'*';j++);
                szBuf1[j] = 0;
                x = swprintf(szBuf2+s2,L"<I>%s</I>",szBuf1+s1);
                s1 += x - 7 + 1;
                s2 += x;
            }
            // [Bold]
            else{
                s1++;
                s1++;
                for(j=s1+1;szBuf1[j] != L'*';j++);
                szBuf1[j] = 0;
                for(j=j+1;szBuf1[j] != L'*';j++);
                szBuf1[j] = 0;
                x = swprintf(szBuf2+s2,L"<B>%s</B>",szBuf1+s1);
                s1 += x - 7 + 2; // 7 == strlen("<B></B>")
                s2 += x;
            }
        }
        else
        // [Code]
        if(szBuf1[s1] == L'`'){
            s1++;
            for(j=s1+1;szBuf1[j] != L'`';j++);
            szBuf1[j] = 0;
            //x = swprintf(szBuf2+s2,L"<CODE>%s</CODE>",szBuf1+s1);
            
            s2 += swprintf(szBuf2+s2,L"<CODE>");
            for(; szBuf1[s1] != 0; s1++){
                switch(szBuf1[s1]){
                case L'<':
                    s2 += swprintf(szBuf2+s2,L"&lt;");
                    break;
                case L'>':
                    s2 += swprintf(szBuf2+s2,L"&gt;");
                    break;
                case L'&':
                    s2 += swprintf(szBuf2+s2,L"&amp;");
                    break;
                default:
                    s2 += swprintf(szBuf2+s2,L"%c",szBuf1[s1]);
                }
            }
            s2 += swprintf(szBuf2+s2,L"</CODE>");

            //s1 += x - 13 + 1;
            s1 += 1;
            //s2 += x;
        }
        else
        // [Link]
        if(szBuf1[s1] == L'['){
            s1++;
            int v = s1;
            for(j=s1;szBuf1[j] != L']';j++);
            szBuf1[j] = 0;
            //[hyperlink syntax](http://markdownpad.com)
            //<a href="http://markdownpad.com">hyperlink syntax</a>

            for(j=j+1;szBuf1[j] != L'(';j++);
            szBuf1[j] = 0;
            int h = j+1;
            for(j=j+1;szBuf1[j] != L')';j++);
            szBuf1[j] = 0;

            x = swprintf(szBuf2+s2,L"<A target=\"_blank\" href=\"%s\">%s</A>",szBuf1+h,szBuf1+v);
            s1 += x - 31 + 3;
            s2 += x;
        }
        else
        // [Img]
        if(szBuf1[s1] == L'!' && szBuf1[s1+1] == L'['){
            s1++;
            s1++;
            int noalt = 0;
            int v = s1;
            for(j=s1;szBuf1[j] != L']';j++);
            if(v == j)noalt = 1;

            szBuf1[j] = 0;
            //[hyperlink syntax](http://markdownpad.com)
            //<a href="http://markdownpad.com">hyperlink syntax</a>

            for(j=j+1;szBuf1[j] != L'(';j++);
            szBuf1[j] = 0;
            int h = j+1;
            for(j=j+1;szBuf1[j] != L')';j++);
            szBuf1[j] = 0;

            if(noalt){
                /*x = swprintf(szBuf2+s2,L"<IMG src=\"%s\" />",szBuf1+h,szBuf1+h);
                s1 += x - 14 + 3;
                */
                
                x = swprintf(szBuf2+s2,L"<IMG src=\"%s\" alt=\"%s\" />",szBuf1+h,szBuf1+h);
                s1 += (x - 21)/2 + 3;
                
            }else{
                x = swprintf(szBuf2+s2,L"<IMG src=\"%s\" alt=\"%s\" />",szBuf1+h,szBuf1+v);
                s1 += x - 21 + 3;
            }
            s2 += x;
        }
        else
        // [Link2]
        if(wcsncmp(szBuf1+s1,L"<http",5) == 0){
            szBuf1[s1] = 0;
            s1++;
            for(j=s1+6;szBuf1[j] != L'>';j++);
            szBuf1[j] = 0;

            x = swprintf(szBuf2+s2,L"<A target=\"_blank\" href=\"%s\">%s</A>",szBuf1+s1,szBuf1+s1);
            s1 += (x - 31)/2 + 1;
            s2 += x;
        }
        else
        // 識別顏符號
        if(wcsncmp(szBuf1+s1,L"-_-|||",6) == 0){
            x = swprintf(szBuf2+s2,L"<NOBR>-_-|||</NOBR>");
            s1 += 6;
            s2 += x;
        }else
        if(wcsncmp(szBuf1+s1,L"-_-||",5) == 0){
            x = swprintf(szBuf2+s2,L"<NOBR>-_-||</NOBR>");
            s1 += 5;
            s2 += x;
        }else
        if(wcsncmp(szBuf1+s1,L"m(_ _)m",7) == 0){
            x = swprintf(szBuf2+s2,L"<NOBR>m(_ _)m</NOBR>");
            s1 += 7;
            s2 += x;
        }else{
        //一般情況
            s2 += swprintf(szBuf2+s2,L"%c",szBuf1[s1++]);
        }
    }
    }else
    if(tag == PRE)
    {
        // [pre]
        // 補充空白行
        if(haveblankline){
            s2 += swprintf(szBuf2+s2,L"\r\n");
        }
        // 原樣複製
        for(; szBuf1[s1] != 0; s1++){
        
            switch(szBuf1[s1]){
            case L'<':
                s2 += swprintf(szBuf2+s2,L"&lt;");
                break;
            case L'>':
                s2 += swprintf(szBuf2+s2,L"&gt;");
                break;
            case L'&':
                s2 += swprintf(szBuf2+s2,L"&amp;");
                break;
            default:
                s2 += swprintf(szBuf2+s2,L"%c",szBuf1[s1]);
            }
        }
        s2 += swprintf(szBuf2+s2,L"\r\n");
        
    }else
    if(tag == P_EX){
        s2 += swprintf(szBuf2+s2,L"<DIV CLASS=\"clear-all\"></DIV>\r\n");
        tag = UNKONW;
    }else
    {
        // tag == DIV || SCRIPT || TABLE || STYLE
        wcscpy(szBuf2+s2,szBuf1+s1);
        wcscat(szBuf2,L"\r\n");
    }

    

    // [p][hn][ul]
    switch(tag){
    case P:
        s2 += swprintf(szBuf2+s2,L" ");
        break;
    case H1:
        s2 += swprintf(szBuf2+s2,L"</H1>\r\n");
        break;
    case H2:
        s2 += swprintf(szBuf2+s2,L"</H2>\r\n");
        break;
    case H3:
        s2 += swprintf(szBuf2+s2,L"</H3>\r\n");
        break;
    case H4:
        s2 += swprintf(szBuf2+s2,L"</H4>\r\n");
        break;
    case H5:
        s2 += swprintf(szBuf2+s2,L"</H5>\r\n");
        break;
    case H6:
        s2 += swprintf(szBuf2+s2,L"</H6>\r\n");
        break;
    case LI:
        s2 += swprintf(szBuf2+s2,L"</LI>\r\n");
        break;
    }

    // [br]
    if(br)s2 += swprintf(szBuf2+s2,L"<BR />\r\n");

    if(wcscmp(szBuf1+s1,L"</DIV>") == 0){
        tag = UNKONW;
    }

    if(wcscmp(szBuf1+s1,L"</SCRIPT>") == 0){
        tag = UNKONW;
    }

    if(wcscmp(szBuf1+s1,L"</TABLE>") == 0){
        tag = UNKONW;
    }

    if(wcscmp(szBuf1+s1,L"</STYLE>") == 0){
        tag = UNKONW;
    }

    lasttag         = tag;
    lastblockquote  = blockquote;
    lastlistindex   = listindex;
    haveblankline   = 0;

    /*
        整些花稍的東西
    */
    switch(tag){
    case P:
        SET_CONSOLE_COLOR(0x07);
        break;
    case H1:
    case H2:
    case H3:
    case H4:
    case H5:
    case H6:
        SET_CONSOLE_COLOR(tag);
        break;
    case LI:
        SET_CONSOLE_COLOR(tag);
        break;
    case STYLE:
    case DIV:
    case SCRIPT:
    case TABLE:
        SET_CONSOLE_COLOR(0x87);
        break;
    case UNKONW:
        break;
    default:
        SET_CONSOLE_COLOR(0xFC);
        break;
    case PRE:
        SET_CONSOLE_COLOR(tag);
        break;
    case P_EX:
        //SET_CONSOLE_COLOR(tag);
        break;
    }

    return 0;
}

int writetofile(FILE *fp){
    wchar_t *str;
    
    str = buf1;
    fwrite(str,sizeof(wchar_t),lstrlenW(str),fp);

    str = buf2;
    fwrite(str,sizeof(wchar_t),lstrlenW(str),fp);

    str = buf3;
    fwrite(str,sizeof(wchar_t),lstrlenW(str),fp);

    return 0;
}

// 測試用例 + 說明文檔
wchar_t str[] = {

L"#Welcome to use "EXENAME L"#\r\n"
L"ver "VERSION L"\r\n"

L"##P B I CODE PRE \r\n"
L"in current version, it support unicode only, sorry for user of utf-8\r\n"
L"\r\n"
L"This is *I* and This is **B**, This is `Code` and This is PRE:\r\n"
L"    while(1)\r\n"
L"    {\r\n"
L"        if( a > 10 && b < 2 )\r\n"
L"            printf('hello');\r\n"
L"    }\r\n"
L"keep word in code and pre tag will be change like `>_<`, "
L"`*` and `[]` can only use in `CODE` tag or it will be analyze."
L"And you will get a `[br /]`  \r\n"
L"if you put two <KBD>space</KBD> in end of a line.\r\n"
L"\r\n"
L"\r\n"
L"You will get a new `[p]` if you keep one or more blank line. You will keep your other `<>` as tag.\r\n"

L"##BlockQuote\r\n"
L">And this is in a qb,  \r\n"
L"in a qb, no need to keep every top a `>`.\r\n"
L">### you can also input use HEADER in qb ###\r\n"
L"### but if no a top `>` in it, you will escape qb.###\r\n"
L">qb can also stack \r\n"
L">>in very level.\r\n"
L">>>>This P have 4 qb.\r\n"
L"\r\n"
L"And use a blank line to escape qb ^_^\r\n"


L"##Here is `UL` example#\r\n"
L"- - - -   This is 4 level UL\r\n"
L"  -       You need not put `-` in head, but if keep too much space, you will take\r\n"
L"    - the <pre>.\r\n"
L" +  and you can alse use`+`\r\n"
L" +  in current version, still no support `OL`, sorry.\r\n"
L" + + + This is 3 Level, Like `qb`, `UL` can be stack.\r\n"
L" + +   This is 2 Level\r\n"


L"##EX-rule#\r\n"
L"your text in `DIV SCRIPT STYLE TABLE` will keep it on:\r\n"
L"<DIV>\r\n"
L">your ![]() **text** in table,script,div and style !tag! will *no* be change! <https://google.com></TABLE>\r\n"
L"</DIV>\r\n"
L"But notice, those tag have to take up full line.\r\n"
L"\r\n"

L"if `;` keep a full line, you will get `<P>&nbsp;<P>`\r\n"
L"    And `;` will force close all opened tag.\r\n"
L";\r\n"
L"    So this is the next <PRE>.\r\n"
L"\r\n"
L"    `\\r\\n` will no do like this.\r\n"

L"\r\n"
L"#####Last, if your text no take a `\\r` and `\\n` in the end, you may lost your last line.\r\n"
L"######Thank your for reading and sorry for my english\r\n"
L"This line will no be show. because no `\\r\\n` at line end."
/**/

};
/*
    a[0] = exefilename
    a[1] = inputfilename
    a[2] = ?
*/


int wmain(int Argc,wchar_t **Argv){

    wchar_t szBuf1[1024];   // input text
    wchar_t szBuf2[1024];   // output text

    wchar_t * szFile1 = Argv[1];
    if(Argv[1] == NULL){
        void showhelp();
        showhelp();
        exit(0);
    }
    wprintf(L"%s\n",szFile1);

    // set new file name
    wchar_t szFile2[80];
    wchar_t * szTok;
    
    wcscpy(szFile2,szFile1);
    szTok = wcsrchr(szFile2,L'.');
    swprintf(szTok,L".html");

    wprintf(L"%s\n",szFile2);

    // read file and wirte
    FILE * fpFile1 = _wfopen(szFile1,L"rb");
    if(fpFile1 == NULL){
        wprintf(L"%s open error!\n",szFile1);
        system("pause");
        exit(0);
    }

    // unicode sign FFFE
    unsigned short hiBuf;
    fread(&hiBuf,sizeof(unsigned short),1,fpFile1);
    if(0xFEFF != hiBuf){
        wprintf(L"this version only support unicode!\n");
        system("pause");
        exit(0);
    }

    FILE * fpFile2 = _wfopen(szFile2,L"wb");
    if(fpFile2 == NULL){
        wprintf(L"%s open error!\n",szFile2);
        exit(0);
    }
    fwrite(&hiBuf,2,1,fpFile2);

    // head
    fwrite(&buf1,sizeof(wchar_t),wcslen(buf1),fpFile2);

    //system("chcp 65001");

    // body
    fwrite(&buf2,sizeof(wchar_t),wcslen(buf2),fpFile2);

    wprintf(L"-------------------------------------------------------------\n");

    int i = -1;
    while(fread(szBuf1+(++i),sizeof(wchar_t),1,fpFile1))
    {
        if(szBuf1[i] == 0x000D || szBuf1[i] == 0x000A/* || szBuf1[i] == EOF*/){
            // end line is 0x0D 0x0A
            if(szBuf1[i] == 0x000A)
            {

                szBuf1[i] = 0;
                szBuf1[i-1] = 0;

                // analyze
                //wprintf(L"[%-20s]",szBuf1);
                AnalyzeOneLine(szBuf1,szBuf2);
                wprintf(L"%s",szBuf2);
                fwrite(&szBuf2,sizeof(wchar_t),wcslen(szBuf2),fpFile2);

                i = -1;
            }
        }else{
            //wprintf(L"%04X ",szBuf1[i],szBuf1[i]);
        }
    }
    // 讀取到 EOF 後，需要設置末尾標記
    szBuf1[i] = 0;
    // BUGFIX: 文件末尾必須保留兩個空行，否則不能被正確的分析
    // `fread`讀到EOF時會返回0，所以需要再讀一次
    //wprintf(L"[%-20s]",szBuf1);
    AnalyzeOneLine(szBuf1,szBuf2);
    wprintf(L"%s",szBuf2);
    fwrite(&szBuf2,sizeof(wchar_t),wcslen(szBuf2),fpFile2);

    // call this after all
    AnalyzeOneLine(NULL,szBuf2);
    wprintf(L"%s",szBuf2);
    fwrite(&szBuf2,sizeof(wchar_t),wcslen(szBuf2),fpFile2);

    //printf("\n");

    SET_CONSOLE_COLOR(0x0F);

    // footer
    fwrite(&buf3,sizeof(wchar_t),wcslen(buf3),fpFile2);
    wprintf(L"-------------------------------------------------------------\n");

    fclose(fpFile1);
    fclose(fpFile2);

    return 0;
}

void showhelp(){
    
    wchar_t szBuf1[1024];   // input text
    wchar_t szBuf2[1024];   // output text

    int i = -1;
    int j = 0;

    // aha, don't change `++i` to `i++`, you knew what i mean.
    while(szBuf1[++i] = str[j++])
    {
        if(szBuf1[i] == 0x000D || szBuf1[i] == 0x000A)
        {
            // end line sign is 0x0D 0x0A
            if(szBuf1[i] == 0x000A)
            {
                szBuf1[i] = 0;
                szBuf1[i-1] = 0;

                // analyze
                // even is a null line, it still have to pass
                AnalyzeOneLine(szBuf1,szBuf2);
                wprintf(L"%s",szBuf2);
                //wprintf(L"------------------------------------------------------------------------\n");
                //getchar();

                i = -1;
            }
        }
    }

    // when read over, call this
    AnalyzeOneLine(NULL,szBuf2);
    wprintf(L"%s\n",szBuf2);

}
