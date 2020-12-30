#include <errno.h>
#include <stdio.h>
#include <iostream>

#include "processor.h"

void Processor::LoadProcStat(
    unsigned long long& user, unsigned long long& nice,
    unsigned long long& system, unsigned long long& idle,
    unsigned long long& iowait, unsigned long long& irq,
    unsigned long long& softirq, unsigned long long& steal,
    unsigned long long& guest, unsigned long long& guestnice) {
  //   unsigned long long user = 0, nice = 0, system = 0, idle = 0;
  //   // added between Linux 2.5.41 and 2.6.33, see man proc(5)
  //   unsigned long long iowait = 0, irq = 0, softirq = 0, steal = 0, guest =
  //   0,
  //                      guestnice = 0;
  FILE* file = fopen("/proc/stat", "r");
  if (file == NULL) {
    perror("Could not open stat file");
    return;
  }

  char buffer[1024];

  char* ret = fgets(buffer, sizeof(buffer) - 1, file);
  if (ret == NULL) {
    perror("Could not read stat file");
    fclose(file);
    return;
  }
  fclose(file);

  sscanf(buffer,
         "cpu  %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu "
         "%16llu",
         &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest,
         &guestnice);
}
// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  unsigned long long cur_user = 0, cur_nice = 0, cur_system = 0, cur_idle = 0,
                     cur_iowait = 0, cur_irq = 0, cur_softirq = 0,
                     cur_steal = 0, cur_guest = 0, cur_guestnice = 0;
  LoadProcStat(cur_user, cur_nice, cur_system, cur_idle, cur_iowait, cur_irq,
               cur_softirq, cur_steal, cur_guest, cur_guestnice);

  auto prev_idle_c = idle + iowait;
  auto cur_idle_c = cur_idle + cur_iowait;

  auto prev_non_idle = user + nice + system + irq + softirq + steal;
  auto cur_non_idle =
      cur_user + cur_nice + cur_system + cur_irq + cur_softirq + cur_steal;

  auto prev_total = prev_idle_c + prev_non_idle;
  auto cur_total = cur_idle_c + cur_non_idle;

  auto total_d = cur_total - prev_total;
  auto idle_d = cur_idle_c - prev_idle_c;

  float cpu_percent = (total_d - idle_d) / (float)total_d;

  user = cur_user;
  nice = cur_nice;
  system = cur_system;
  idle = cur_idle;
  iowait = cur_iowait;
  irq = cur_irq;
  softirq = cur_softirq;
  steal = cur_steal;
  guest = cur_guest;
  guestnice = cur_guestnice;
  return cpu_percent;
}