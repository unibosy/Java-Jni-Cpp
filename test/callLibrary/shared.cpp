#include <stdio.h>
#include "computer.h"
#include <iostream>
#include "shared.h"

void shared_print()
{
	int a=3, b=5;
	printf("%d + %d = %d\n", a, b, my_add(a, b));
	printf("%d * %d = %d\n", a, b, my_mul(a, b));
}
