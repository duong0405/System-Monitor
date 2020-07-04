#include "processor.h"
#include "linux_parser.h"

// Return the aggregate CPU utilization
float Processor::Utilization()
{
    long total_new = CurrentTotal();
    long active_new = CurrentActive();
    long idle_new = CurrentIdle();

    long total_old = PrevTotal();
    long idle_old = PrevIdle();

    Update(idle_new, active_new, total_new);

    float totalDelta = static_cast<float>(total_new) - static_cast<float>(total_old);
    float idleDelta = static_cast<float>(idle_new) - static_cast<float>(idle_old);

    float utilization = (totalDelta - idleDelta) / totalDelta;
    if (utilization > 0) return utilization;
    return 0;
}

long Processor::CurrentTotal() { return LinuxParser::Jiffies(); }
long Processor::CurrentActive() { return LinuxParser::ActiveJiffies(); }
long Processor::CurrentIdle() { return LinuxParser::IdleJiffies(); }

long Processor::PrevTotal() { return m_total; }
long Processor::PrevActive() { return m_active; }
long Processor::PrevIdle() { return m_idle; }

void Processor::Update(long idle, long active, long total) 
{
  m_idle = idle;
  m_active = active;
  m_total = total;
} 