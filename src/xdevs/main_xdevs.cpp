
#include <xdevs/core/simulation/Coordinator.h>

#include "DevsSlide.h"

int main() {
	DevsSlide dataCenter("DevsSlide", "test/WeatherFile.txt");
	Coordinator coordinator(&dataCenter);
	coordinator.initialize();
	coordinator.simulate((long int)100000000);
	coordinator.exit();
	return 0;
} 
