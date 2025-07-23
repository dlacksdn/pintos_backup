#include "threads/thread.h"
#include "devices/timer.h"
#include "threads/interrupt.h"
#include <stdio.h>

#define N 100
#define RUNNING_TIME 100  // ticks (약 1초)
#define REPEAT 1000

extern int count_stride[];
static volatile bool running = true;
static volatile int create_count;

// rdtsc 정의
static inline uint64_t rdtsc(void) {
  unsigned int lo, hi;
  __asm__ volatile ("rdtsc" : "=a"(lo), "=d"(hi));
  return ((uint64_t)hi << 32) | lo;
}

static volatile void
thread_func_perf(void *aux) {
  int id = *(int *)aux;
    // 이 줄 추가: 현재 실행 중인 스레드에 ID 기록
  thread_current()->perf_id = id;

  while (running) {
     
  }

  thread_exit();
}

void test_stride_sequential_search(void) {
  set_scheduler(SCHED_STRIDE_SEQ);

  int ticket_num[N] = {10, 20, 40, 50, 100};
  int id[N];

  if (N != 5) {
    for(int i = 0; i < N; i++) {
      ticket_num[i] = (i + 1) * 10;
    }
  } 
  
  enum intr_level old = intr_disable();
  for (create_count = 0; create_count < N; create_count++) {
    id[create_count] = create_count;
    char name[16];

    snprintf (name, sizeof name, "thread %d", create_count);
    thread_create_stride(name, PRI_DEFAULT, ticket_num[create_count], thread_func_perf, &id[create_count]);
  }
  intr_set_level(old);


  timer_sleep(RUNNING_TIME);
  running = false;

  int total = 0;
  for (int i = 0; i < N; i++)
    total += count_stride[i];

  /* 출력 */
  printf("Stride Performance Result:\n");
  for (int i = 0; i < N; i++) {
    int cnt = count_stride[i];
    /* ‰ 단위 비율 = cnt * 1000 / total */
    int permille = (total > 0 ? cnt * 1000 / total : 0);
    int percent = permille / 10;      /* 정수 부분 (%) */
    int frac    = permille % 10;      /* 소수 첫째 자리 */
    printf("thread %d (tickets=%d) ran %d times (%d.%d%%)\n",
          i, ticket_num[i], cnt, percent, frac);
  }

  // [3] pick_lottery_thread() 성능 측정
  printf("\nMeasuring pick_stride_seq_thread() with %d threads...\n", N);
  uint64_t total_performance = 0;
  for (int i = 0; i < REPEAT; i++) {
    old = intr_disable();
    uint64_t start = rdtsc();
    pick_stride_seq_thread();  // 티켓 기반 추첨 함수
    uint64_t end = rdtsc();
    intr_set_level(old);
    total_performance += (end - start);
  }

  printf(" average stride selection time: %llu cycles\n", total_performance / REPEAT);
}


