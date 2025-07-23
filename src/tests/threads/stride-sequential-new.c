#include "threads/thread.h"
#include "devices/timer.h"
#include <stdio.h>
#include <inttypes.h>    /* PRId64 매크로를 위해 */

#define N 5
#define RUNNING_TIME 10000  // 100 ticks가 약 1초

extern int count_stride_new[];
static volatile bool running = true;
extern bool is_late_arrival;

static volatile void
thread_func_perf(void *aux) {
  int id = *(int *)aux;
    // 이 줄 추가: 현재 실행 중인 스레드에 ID 기록
  thread_current()->perf_id = id;

  while (running) {
    
  }

  thread_exit();
}

void test_stride_sequential_search_new(void) {
  set_scheduler(SCHED_STRIDE_SEQ_NEW);

  int ticket_num[N] = {10, 20, 40, 50, 100};
  int id[N];

  for (int i = 0; i < N; i++)
    id[i] = i;
  
  volatile int j;
  char name[16];
  is_late_arrival = false;

  for (j = 0; j < N - 1; j++) {
    snprintf (name, sizeof name, "thread %d", j);
    thread_create_stride(name, PRI_DEFAULT, ticket_num[j], thread_func_perf, &id[j]);
  }

  is_late_arrival = true;
  timer_sleep(RUNNING_TIME / 2);

  snprintf (name, sizeof name, "thread %d", j);
  thread_create_stride(name, PRI_DEFAULT, ticket_num[j], thread_func_perf, &id[j]);

  timer_sleep(RUNNING_TIME / 2);
  running = false;

  int total = 0;
  for (int i = 0; i < N; i++)
    total += count_stride_new[i];

  /* 출력 */
  printf("Stride Performance Result:\n");
  for (int i = 0; i < N; i++) {
    int cnt = count_stride_new[i];
    /* ‰ 단위 비율 = cnt * 1000 / total */
    int permille = (total > 0 ? cnt * 1000 / total : 0);
    int percent = permille / 10;      /* 정수 부분 (%) */
    int frac    = permille % 10;      /* 소수 첫째 자리 */
    printf("thread %d (tickets=%d) ran %d times (%d.%d%%)\n",
          i, ticket_num[i], cnt, percent, frac);
  }
}


