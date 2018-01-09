#include <stdio.h>
#include <stdint.h>

uint64_t time_load(void* o);
void cache_flush(void* o);
void run_attempt(void);

uint64_t buffer[4096];
uint64_t dummy_buffer[4096];

int bools[1024];
uint8_t *pointers[1024];
uint8_t values[1024];

int main() {
	uint64_t *p = &buffer[4096/2];
	uint64_t *dummy_p = &dummy_buffer[4096/2];

	for (int i = 0; i < 1024; i++) {
		// if you set this value to one, the branch will correctly predict
		// to skip pointers[1000]. this disables speculative execution (and
		// demonstrates a "false" result).
		bools[i] = 0;
		pointers[i] = dummy_p;
		if (i == 1000) {
			bools[i] = 1;
			pointers[i] = p;
		}
	}

#define NUM_ATTEMPTS 20
	uint64_t times[NUM_ATTEMPTS];
	for (int attempt = 0; attempt < NUM_ATTEMPTS; attempt++) {
		cache_flush(p);

		run_attempt();

		times[attempt] = time_load(p);
	}

	printf("bools: %p\n", &bools[0]);
	uint64_t min = times[0];
	for (int attempt = 0; attempt < NUM_ATTEMPTS; attempt++) {
		printf("time: %llu!\n", times[attempt]);
		if (times[attempt] < min) {
			min = times[attempt];
		}
	}
	printf("min: %llu!\n", min);


	return 0;
}