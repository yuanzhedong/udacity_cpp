#ifndef PROCESSOR_H
#define PROCESSOR_H
#include <tuple>

class Processor {
 public:
  void LoadProcStat(unsigned long long&, unsigned long long&,
                    unsigned long long&, unsigned long long&,
                    unsigned long long&, unsigned long long&,
                    unsigned long long&, unsigned long long&,
                    unsigned long long&, unsigned long long&);
  Processor() {
    LoadProcStat(user, nice, system, idle, iowait, irq, softirq, steal, guest,
                 guestnice);
  }

  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  unsigned long long user;
  unsigned long long nice;
  unsigned long long system;
  unsigned long long idle;
  unsigned long long iowait;
  unsigned long long irq;
  unsigned long long softirq;
  unsigned long long steal;
  unsigned long long guest;
  unsigned long long guestnice;
};

#endif