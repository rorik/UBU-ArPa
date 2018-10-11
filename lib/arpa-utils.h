#pragma once
#include <time.h>
int squarestFactor(int number) {
	if (number <= 0) {
		if (number == 0) {
			return 1;
		}
		else {
			number = -number;
		}
	}
	for (int i = (int)sqrt(number);; i++) {
		if (number % i == 0) {
			int otherFactor = abs(number / i);
			return otherFactor >= i ? otherFactor : i;
		}
	}
}

void randomizeSeed() {
	srand((unsigned int)time(NULL));
}