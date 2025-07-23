#include "threads/thread.h"
#include "devices/timer.h"
#include <stdio.h>

#define N 5
#define RUNNING_TIME 5000  // 100 ticks가 약 1초

extern int count_stride_priority[];
static volatile bool running = true;

static volatile void
thread_func_perf(void *aux) {
  int id = *(int *)aux;
    // 이 줄 추가: 현재 실행 중인 스레드에 ID 기록
  thread_current()->perf_id = id;

  while (running) {
    
  }

  thread_exit();
}

void test_stride_sequential_search_priority(void) {
  set_scheduler(SCHED_STRIDE_SEQ_PRIORITY);

  int priority_num[N] = {60, 2, 4, 12, 4};
  int ticket_num[N] = {10, 20, 40, 50, 100};
  int id[N];

  // 1, 2, 4, 5, 8, 10, 20, 25, 40, 50, 100, 125, 200, 250, 500, 1000 
  // 200 20 80 100 200
  // 600 40 160 600 400
  // 15 1 4 15 10

  for (int i = 0; i < N; i++)
    id[i] = i;
  
  
  for (int i = 0; i < N; i++) {
    char name[16];

    snprintf (name, sizeof name, "thread %d", i);
    thread_create_stride(name, priority_num[i], ticket_num[i], thread_func_perf, &id[i]);
  }

  timer_sleep(RUNNING_TIME);
  running = false;

  int total = 0;
  for (int i = 0; i < N; i++)
    total += count_stride_priority[i];

  /* 출력 */
  printf("Stride Performance Result:\n");
  for (int i = 0; i < N; i++) {
    int cnt = count_stride_priority[i];
    /* ‰ 단위 비율 = cnt * 1000 / total */
    int permille = (total > 0 ? cnt * 1000 / total : 0);
    int percent = permille / 10;      /* 정수 부분 (%) */
    int frac    = permille % 10;      /* 소수 첫째 자리 */
    printf("thread %d (tickets=%d) ran %d times (%d.%d%%)\n",
          i, ticket_num[i], cnt, percent, frac);
  }
}


