#typeof marco on C

    #define typeof(_Val)    (sizeof((_Val)+(float)0)==sizeof(float) ? (                                 \
                            (_Val) != ((int)_Val) || (_Val-_Val+~(unsigned int)0+1) > ((int)0) ? 'f' :  \
                            ((_Val) < 0 || (_Val)+(1<<31) < 0) ? 'i' : 'u' ):                           \
                            sizeof ((_Val) + (double)0) == sizeof (double) ? 'd' : 'l')

Similar to `getName()` on java or `typeof` on javascript, this macro get the type of variable defined on C.

return a char：

    *f*loat *d*ouble *i*nt or *u*nsigned

I was inspired in math.h#280, it look like:

    #define _CLASS_ARG(_Val) (sizeof ((_Val) + (float)0) == sizeof (float) ? 'f' :\
                              sizeof ((_Val) + (double)0) == sizeof (double) ? 'd' : 'l')

This macro can detect float double and long double by checking its size.
On this basis, I added the detection of signed and unsigned.

First, let me tell you how to detect signed or unsigned.

Here is my fake code:

    x = signed or unsigned;
    
    if x < 0, x must be signed.
    else if x > 0,
        if x is signed then x highest must be 0,
        if x is unsigned, it's highest can be 0 or 1(but I don't care).
    
    now, let's set the highest to be 1. um, actually, I can only sure signed int can be set to 1, unsigned still be 0 or 1(but i still don't care).
    
    if the finaly value < 0, then it must be signed.
    else, it is unsigned.
    
Let's change the fake code to marco:

    #define isunsigned(_Val)   (((_Val) < 0 || (_Val)+(1<<(sizeof(int)*8-1)) < 0) ? 'i' : 'u')

there have another way:

    #define isunsigned(v)   (((v) < 0 || (v) > 0 && ~(v) < 0) ? 'i' : 'u')
    
way I not use? opreator `~` can not use in double or float!

And now, let's have a check, here is my check code:

    #include <assert.h>
    #define isunsigned(_Val)   (((_Val) < 0 || (_Val)+(1<<(sizeof(int)*8-1)) < 0) ? 'i' : 'u')
    int main(){
        for(unsigned int k=0;k<0xFFFFFFFFFFFFFFFF;k++){
            int         i=(k-(unsigned)0x7FFFFFFFFFFFFFF);
            unsigned    u=k;
            assert(isunsigned(i)=='i');
            assert(isunsigned(u)=='u');
        }
    }
    
be careful, if you run this code in release mode, it won't work!
and this program may run about 3 hours.

Leave it alone, let me tell you how to distinguish between float and integer.

Here is my fake code:
    
    x is a variable;
    
    if sizeof x is sizeof float and 
    if x is not a integer, it must be float.
    and then, i have to do a bad thing:
    
    set x to 0 and add to the largest integer(maybe 0xFFFFFFFFFFFFFFFF).
    
    now, the result have save in a temp variable, call it y.
    you have to konw, x and y have the same type and float can hold a larger number than the largest integer.
    
    so, let y add to 1.
    
    if y is a integer, it will overflow to 0, but float can continue hold the value.
    
The marco is same as beginning.
    
Thank you for your reading!