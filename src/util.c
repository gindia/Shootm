#include "common.h"
#include "structs.h"

#if 0
inline static int MIN(int a,int b) { return (((a)<(b))?(a):(b)); }
inline static int MAX(int a,int b) { return (((a)>(b))?(a):(b)); }
static int collison(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
    return (MAX(x1, x2) < MIN((x1 + w1), (x2 + w2))) && (MAX(y1, y2) < MIN((y1 + h1), (y2 + h2)));
}
#endif

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

inline static int collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
	return (MAX(x1, x2) < MIN(x1 + w1, x2 + w2)) && (MAX(y1, y2) < MIN(y1 + h1, y2 + h2));
}

inline static void calc_slope(int x1, int y1, int x2, int y2, float *dx, float *dy)
{
    int steps = MAX(abs(x1 - x2), abs(y1 - y2));
    if(steps == 0)
    {
        *dx = *dy = 0;
        return;
    }

    *dx  = (x1 - x2);
    *dx /= steps;

    *dy  = (y1 - y2);
    *dy /= steps;
}
