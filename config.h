
// TODO: follow commenting pattern from first t2 defintions
// --- ADMIN STUFF ---
#define LOG_PBS 1
#define MAX_NUMBER_PROCESSES 10
#define MAX_NUMBER_TASKS_IN_PLAN 400
#define MAX_LEN_PLAN_STR 20000
#define ACCURACY 10000

// --- GENERAL ---
#define PBS_HZ 100
#define INS_PER_SEC 1000000000
#define INS_PER_TICK (INS_PER_SEC/PBS_HZ)
#define RESCHEDULE_TIME ((long)PBS_HZ*INS_PER_TICK)

// --- T1 ---
#define T1_MAX_TICKS_OFF 10
#define T1_MIN_TICKS_OFF 3
#define PREEMPTION_LIMIT T1_MAX_TICKS_OFF * INS_PER_TICK
#define T1_NO_PREEMPTION T1_MIN_TICKS_OFF * INS_PER_TICK
#define T1_SIGMA 125

// --- T2 ---
#define T2_SIGMA (T1_SIGMA + 50) // percentage as int; max allowed deviation % of a task from its plan
#define T2_SPACER (T1_MAX_TICKS_OFF * INS_PER_TICK ) // raw number instructions; Distance t1 -> t2_task
#define T2_TASK_SIGNALING_LIMIT (PBS_HZ * INS_PER_TICK)// raw number instructions; t2_task max value TODO: Implement

#define T2_CAPACITY_BUFFER 110 // percentage as int, underestimation of node computational capacity
#define T2_ASSIGNABLE_PLAN_BUFFER 50 // Factor that describes what percentage of the buffer may be used up, e.g. 50 with a 1000 buffer means, that only a buffer of 500 may be used before a prediction failure will be send
#define T2_PROCESS_MINIMUM ((long)(RESCHEDULE_TIME * 110)/100)

#define T2_NODE_LATENESS 110

#define T2_MAX_PREEMPTIONS 5

#define STRESS_RESET (PBS_HZ*30)
#define T2_STRESS_GAIN ((25 * INS_PER_TICK)/100)


// --- Tm2 ---
// Tm2 Task
#define TM2_SIGMA (100 - (T2_SIGMA - 100))
#define TM2_TASK_SIGNALING_LIMIT (T2_SPACER * -1)
#define TM2_TASK_SIGNALING_START (T2_TASK_SIGNALING_LIMIT * -1)

// Tm2 Node
#define TM2_NODE_EARLINESS_CAP (-1*(100 - (T2_NODE_LATENESS - 100)))
#define TM2_NODE_LOWER_BOUND -20

// --- RESCHEDULING ---
#define STRETCH_CONSTANT 105 // percentage as int; determines how much tasks are stretched by rescheduling
#define SHRINK_CONSTANT 95 // percentage as int; determines how much tasks are shrunk by rescheduling

// --- enable/disable thresholds ---

#define T1_ENABLED 1
#define T2_TASK_ENABLED 1
#define TM2_TASK_ENABLED 1

#define T2_PROCESS_ENABLED 1
#define TM2_PROCESS_ENABLED 1

#define T2_NODE_ENABLED 1
#define TM2_NODE_ENABLED 1

#define T2_PREEMPTIONS_ENABLED 1