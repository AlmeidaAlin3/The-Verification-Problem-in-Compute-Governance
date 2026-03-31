#define _GNU_SOURCE
#include "measure.h"

sampler_config_t config;

uint64_t *timestamps_us = NULL;
uint32_t *power_mw_data = NULL;
uint32_t *sm_clock_data = NULL;
uint32_t *mem_clock_data = NULL;
uint32_t *gpu_util_data = NULL;
uint32_t *pcie_tx_data = NULL;
uint32_t *temperature_data = NULL;

int sample_count = 0;

static nvmlDevice_t device;

void sig_handler(int sig) {
    (void)sig;
    config.running = 0;
}

uint64_t now_monotonic_us(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ((uint64_t)ts.tv_sec * 1000000ULL) + (ts.tv_nsec / 1000ULL);
}

int sleep_until_us(uint64_t target_us) {
    struct timespec ts;
    ts.tv_sec = target_us / 1000000ULL;
    ts.tv_nsec = (target_us % 1000000ULL) * 1000ULL;
    return clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, NULL);
}

static void try_set_affinity_and_priority(void) {
    if (config.cpu_core >= 0) {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(config.cpu_core, &cpuset);

        if (sched_setaffinity(0, sizeof(cpu_set_t), &cpuset) != 0) {
            fprintf(stderr, "Warning: could not set CPU affinity to core %d: %s\n",
                    config.cpu_core, strerror(errno));
        }
    }

    if (setpriority(PRIO_PROCESS, 0, config.nice_value) != 0) {
        fprintf(stderr, "Warning: could not set nice value to %d: %s\n",
                config.nice_value, strerror(errno));
    }
}

static void check_nvml(nvmlReturn_t r, const char* what) {
    if (r != NVML_SUCCESS) {
        fprintf(stderr, "NVML error in %s: %s\n", what, nvmlErrorString(r));
    }
}

void *threadWork(void *arg) {
    (void)arg;

    try_set_affinity_and_priority();

    const uint64_t period_us = (uint64_t)config.sample_time_ms * 1000ULL;
    uint64_t t0 = now_monotonic_us();
    uint64_t next_deadline = t0;

    while (config.running && sample_count < config.max_samples) {
        uint32_t power_mw = 0;
        uint32_t sm_clock = 0;
        uint32_t mem_clock = 0;
        uint32_t temp = 0;
        nvmlUtilization_t util = {0};
        uint32_t pcie_tx = 0;

        uint64_t ts = now_monotonic_us();

        check_nvml(nvmlDeviceGetPowerUsage(device, &power_mw), "nvmlDeviceGetPowerUsage");
        check_nvml(nvmlDeviceGetClockInfo(device, NVML_CLOCK_SM, &sm_clock), "nvmlDeviceGetClockInfo(SM)");
        check_nvml(nvmlDeviceGetClockInfo(device, NVML_CLOCK_MEM, &mem_clock), "nvmlDeviceGetClockInfo(MEM)");
        check_nvml(nvmlDeviceGetUtilizationRates(device, &util), "nvmlDeviceGetUtilizationRates");
        check_nvml(nvmlDeviceGetPcieThroughput(device, NVML_PCIE_UTIL_TX_BYTES, &pcie_tx), "nvmlDeviceGetPcieThroughput");
        check_nvml(nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &temp), "nvmlDeviceGetTemperature");

        timestamps_us[sample_count] = ts;
        power_mw_data[sample_count] = power_mw;     // keep mW resolution
        sm_clock_data[sample_count] = sm_clock;
        mem_clock_data[sample_count] = mem_clock;
        gpu_util_data[sample_count] = util.gpu;
        pcie_tx_data[sample_count] = pcie_tx;
        temperature_data[sample_count] = temp;

        sample_count++;

        next_deadline += period_us;

        if (sleep_until_us(next_deadline) != 0 && errno != EINTR) {
            fprintf(stderr, "Warning: clock_nanosleep failed: %s\n", strerror(errno));
        }
    }

    return NULL;
}

void save_sensor_data_u32(
    const char* name,
    uint32_t* data,
    int n,
    uint64_t* timestamps,
    const char* unit
) {
    char filename[128];
    snprintf(filename, sizeof(filename), "telemetry_%s_data.csv", name);

    FILE *f = fopen(filename, "w");
    if (!f) {
        fprintf(stderr, "Error opening %s for writing.\n", filename);
        return;
    }

    fprintf(f, "sep=;\n");
    fprintf(f, "timestamp_us;%s_%s\n", name, unit);

    for (int i = 0; i < n; i++) {
        fprintf(f, "%llu;%u\n",
                (unsigned long long)timestamps[i],
                data[i]);
    }

    fclose(f);

    char summary[128];
    snprintf(summary, sizeof(summary), "telemetry_%s_summary.txt", name);

    FILE *s = fopen(summary, "w");
    if (!s) {
        fprintf(stderr, "Error opening %s for writing.\n", summary);
        return;
    }

    uint32_t min = data[0];
    uint32_t max = data[0];
    double avg = 0.0;

    for (int i = 0; i < n; i++) {
        if (data[i] < min) min = data[i];
        if (data[i] > max) max = data[i];
        avg += data[i];
    }

    avg /= (double)n;

    fprintf(s, "Average,%f\nMin,%u\nMax,%u\nSamples,%d\n", avg, min, max, n);
    fclose(s);
}

void save_metadata(
    const char* command_line,
    int target_period_ms,
    int actual_samples,
    int cpu_core,
    int nice_value
) {
    FILE *f = fopen("telemetry_metadata.txt", "w");
    if (!f) {
        fprintf(stderr, "Error opening telemetry_metadata.txt for writing.\n");
        return;
    }

    fprintf(f, "target_period_ms=%d\n", target_period_ms);
    fprintf(f, "target_frequency_hz=%.3f\n", 1000.0 / (double)target_period_ms);
    fprintf(f, "sample_count=%d\n", actual_samples);
    fprintf(f, "cpu_core=%d\n", cpu_core);
    fprintf(f, "nice_value=%d\n", nice_value);
    fprintf(f, "command=%s\n", command_line ? command_line : "");
    fclose(f);
}

static void free_all(void) {
    free(timestamps_us);
    free(power_mw_data);
    free(sm_clock_data);
    free(mem_clock_data);
    free(gpu_util_data);
    free(pcie_tx_data);
    free(temperature_data);
}

static int parse_args(int argc, char **argv, int *cmd_index) {
    config.sample_time_ms = SAMPLE_TIME_MS_DEFAULT;
    config.max_samples = SAMPLE_MAX_SIZE_DEFAULT;
    config.running = 1;
    config.cpu_core = -1;
    config.nice_value = -20;

    *cmd_index = -1;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            config.sample_time_ms = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            config.max_samples = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
            config.cpu_core = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            config.nice_value = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-a") == 0 && i + 1 < argc) {
            *cmd_index = i + 1;
            return 0;
        } else {
            fprintf(stderr, "Unknown argument: %s\n", argv[i]);
            return -1;
        }
    }

    return (*cmd_index == -1) ? -1 : 0;
}

int main(int argc, char **argv) {
    int cmd_index = -1;

    if (parse_args(argc, argv, &cmd_index) != 0) {
        fprintf(stderr,
                "Usage: %s -s <ms> [-n max_samples] [-c cpu_core] [-p nice] -a <command> [args...]\n",
                argv[0]);
        return 1;
    }

    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    timestamps_us   = malloc(sizeof(uint64_t) * config.max_samples);
    power_mw_data   = malloc(sizeof(uint32_t) * config.max_samples);
    sm_clock_data   = malloc(sizeof(uint32_t) * config.max_samples);
    mem_clock_data  = malloc(sizeof(uint32_t) * config.max_samples);
    gpu_util_data   = malloc(sizeof(uint32_t) * config.max_samples);
    pcie_tx_data    = malloc(sizeof(uint32_t) * config.max_samples);
    temperature_data= malloc(sizeof(uint32_t) * config.max_samples);

    if (!timestamps_us || !power_mw_data || !sm_clock_data || !mem_clock_data ||
        !gpu_util_data || !pcie_tx_data || !temperature_data) {
        fprintf(stderr, "Error: memory allocation failed.\n");
        free_all();
        return 1;
    }

    nvmlReturn_t r = nvmlInit();
    if (r != NVML_SUCCESS) {
        fprintf(stderr, "Error initializing NVML: %s\n", nvmlErrorString(r));
        free_all();
        return 1;
    }

    r = nvmlDeviceGetHandleByIndex(0, &device);
    if (r != NVML_SUCCESS) {
        fprintf(stderr, "Error getting GPU 0 handle: %s\n", nvmlErrorString(r));
        nvmlShutdown();
        free_all();
        return 1;
    }

    pthread_t thread;
    if (pthread_create(&thread, NULL, threadWork, NULL) != 0) {
        fprintf(stderr, "Error creating sampling thread.\n");
        nvmlShutdown();
        free_all();
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        fprintf(stderr, "Fork error.\n");
        config.running = 0;
        pthread_join(thread, NULL);
        nvmlShutdown();
        free_all();
        return 1;
    }

    if (pid == 0) {
        execvp(argv[cmd_index], &argv[cmd_index]);
        fprintf(stderr, "Error executing command '%s': %s\n",
                argv[cmd_index], strerror(errno));
        _exit(127);
    }

    int status = 0;
    waitpid(pid, &status, 0);

    config.running = 0;
    pthread_join(thread, NULL);

    printf("Saving telemetry...\n");

    if (sample_count > 0) {
        save_sensor_data_u32("power_mw", power_mw_data, sample_count, timestamps_us, "mW");
        save_sensor_data_u32("sm_clock", sm_clock_data, sample_count, timestamps_us, "MHz");
        save_sensor_data_u32("mem_clock", mem_clock_data, sample_count, timestamps_us, "MHz");
        save_sensor_data_u32("gpu_util", gpu_util_data, sample_count, timestamps_us, "pct");
        save_sensor_data_u32("pcie_tx", pcie_tx_data, sample_count, timestamps_us, "KBps");
        save_sensor_data_u32("temperature", temperature_data, sample_count, timestamps_us, "C");

        char command_line[4096] = {0};
        size_t used = 0;
        for (int i = cmd_index; i < argc; i++) {
            int written = snprintf(command_line + used, sizeof(command_line) - used,
                                   "%s%s", (i == cmd_index ? "" : " "), argv[i]);
            if (written < 0 || (size_t)written >= sizeof(command_line) - used) break;
            used += (size_t)written;
        }

        save_metadata(command_line, config.sample_time_ms, sample_count,
                      config.cpu_core, config.nice_value);
    } else {
        fprintf(stderr, "Warning: no samples were collected.\n");
    }

    printf("Done.\n");

    nvmlShutdown();
    free_all();

    return (WIFEXITED(status) ? WEXITSTATUS(status) : 1);
}
