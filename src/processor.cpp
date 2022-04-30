#include "processor.h"

#include "linux_parser.h"

// https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
// https://www.anshulpatel.in/post/linux_cpu_percentage/

float Processor::Utilization() {
  float cpu_percentage;

  long idle_jiffies = LinuxParser::IdleJiffies();
  long active_jiffies = LinuxParser::ActiveJiffies();

  long previous_total = previous_idle_jiffies + previous_active_jiffies;
  long total_jiffies = idle_jiffies + active_jiffies;

  long d_total = total_jiffies - previous_total;
  long d_idle = idle_jiffies - previous_idle_jiffies;

  previous_idle_jiffies = idle_jiffies;
  previous_active_jiffies = active_jiffies;

  cpu_percentage = float(d_total - d_idle) / float(d_total);

  return cpu_percentage;
}