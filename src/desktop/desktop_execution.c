#include "desktop_execution.h"

#include "beast2/app.h"
#include "beast2/config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif

typedef struct desktop_job_payload {
    char config_path[BEAST2_MAX_PATH_LENGTH];
    int step_count;
    char paths[BEAST2_DESKTOP_PIPELINE_MAX_STEPS][BEAST2_MAX_PATH_LENGTH];
} desktop_job_payload;

static char s_config_path[BEAST2_MAX_PATH_LENGTH];
static char s_generator_path[BEAST2_MAX_PATH_LENGTH];
static char s_pipeline[BEAST2_DESKTOP_PIPELINE_MAX_STEPS][BEAST2_MAX_PATH_LENGTH];
static int s_pipeline_step_count;
static char s_status_line[512];
static int s_running;
static int s_completed_pulse;

#if defined(_WIN32)
static CRITICAL_SECTION s_lock;
static HANDLE s_thread_handle;
#else
static pthread_mutex_t s_mutex;
static pthread_t s_thread;
static int s_pthread_joinable;
#endif

static void desktop_execution_lock(void) {
#if defined(_WIN32)
    EnterCriticalSection(&s_lock);
#else
    pthread_mutex_lock(&s_mutex);
#endif
}

static void desktop_execution_unlock(void) {
#if defined(_WIN32)
    LeaveCriticalSection(&s_lock);
#else
    pthread_mutex_unlock(&s_mutex);
#endif
}

static void desktop_join_previous_worker(void) {
#if defined(_WIN32)
    if (s_thread_handle != NULL) {
        WaitForSingleObject(s_thread_handle, INFINITE);
        CloseHandle(s_thread_handle);
        s_thread_handle = NULL;
    }
#else
    if (s_pthread_joinable != 0) {
        pthread_join(s_thread, NULL);
        s_pthread_joinable = 0;
    }
#endif
}

#if defined(_WIN32)
static unsigned __stdcall desktop_worker_thread(void *arg) {
#else
static void *desktop_worker_thread(void *arg) {
#endif
    desktop_job_payload *job = (desktop_job_payload *) arg;
    beast2_execution_summary summary;
    char err[512];
    int rc = 0;
    int step;

    memset(&summary, 0, sizeof(summary));
    memset(err, 0, sizeof(err));

    for (step = 0; step < job->step_count; step++) {
        memset(&summary, 0, sizeof(summary));
        memset(err, 0, sizeof(err));

        rc = beast2_run_generator_with_summary(
            job->config_path,
            job->paths[step],
            &summary,
            err,
            sizeof(err)
        );

        if (rc != 0) {
            desktop_execution_lock();
            snprintf(
                s_status_line,
                sizeof s_status_line,
                "Pipeline step %d/%d failed: %s",
                step + 1,
                job->step_count,
                err
            );
            s_running = 0;
            s_completed_pulse = 1;
            desktop_execution_unlock();
            free(job);

#if defined(_WIN32)
            _endthreadex(0);
            return 0;
#else
            return NULL;
#endif
        }
    }

    desktop_execution_lock();
    if (job->step_count > 1) {
        snprintf(
            s_status_line,
            sizeof s_status_line,
            "Pipeline ok (%d steps) | %s | jobs %zu/%zu",
            job->step_count,
            summary.generator_name,
            summary.completed_jobs,
            summary.total_jobs
        );
    } else {
        snprintf(
            s_status_line,
            sizeof s_status_line,
            "Jobs: %zu/%zu ok | queue peak %zu | %s",
            summary.completed_jobs,
            summary.total_jobs,
            summary.scheduler_peak_queue_length,
            summary.generator_name
        );
    }

    s_running = 0;
    s_completed_pulse = 1;
    desktop_execution_unlock();

    free(job);

#if defined(_WIN32)
    _endthreadex(0);
    return 0;
#else
    return NULL;
#endif
}

void desktop_execution_init(void) {
    s_config_path[0] = '\0';
    s_generator_path[0] = '\0';
    s_pipeline_step_count = 0;
    snprintf(s_status_line, sizeof s_status_line, "%s", "Jobs: idle");
    s_running = 0;
    s_completed_pulse = 0;
#if defined(_WIN32)
    InitializeCriticalSection(&s_lock);
    s_thread_handle = NULL;
#else
    pthread_mutex_init(&s_mutex, NULL);
    s_pthread_joinable = 0;
#endif
}

void desktop_execution_shutdown(void) {
    desktop_join_previous_worker();
#if defined(_WIN32)
    DeleteCriticalSection(&s_lock);
#else
    pthread_mutex_destroy(&s_mutex);
#endif
}

void desktop_execution_set_paths_pipeline(
    const char *config_path,
    const char *const *generator_paths,
    int step_count
) {
    int i;

    if (config_path != NULL) {
        snprintf(s_config_path, sizeof s_config_path, "%s", config_path);
    } else {
        s_config_path[0] = '\0';
    }

    s_pipeline_step_count = 0;
    s_generator_path[0] = '\0';

    if (generator_paths == NULL || step_count < 1) {
        return;
    }

    if (step_count > BEAST2_DESKTOP_PIPELINE_MAX_STEPS) {
        step_count = BEAST2_DESKTOP_PIPELINE_MAX_STEPS;
    }

    for (i = 0; i < step_count; i++) {
        if (generator_paths[i] == NULL || generator_paths[i][0] == '\0') {
            break;
        }
        snprintf(s_pipeline[i], sizeof s_pipeline[i], "%s", generator_paths[i]);
        s_pipeline_step_count++;
    }

    if (s_pipeline_step_count > 0) {
        snprintf(s_generator_path, sizeof s_generator_path, "%s", s_pipeline[0]);
    }
}

void desktop_execution_set_paths(const char *config_path, const char *generator_path) {
    const char *one[1];

    if (generator_path == NULL) {
        desktop_execution_set_paths_pipeline(config_path, NULL, 0);
        return;
    }

    one[0] = generator_path;
    desktop_execution_set_paths_pipeline(config_path, one, 1);
}

int desktop_execution_try_start(void) {
    desktop_job_payload *job = NULL;
    int pi;
    int n;

    desktop_join_previous_worker();

    desktop_execution_lock();
    if (s_running) {
        desktop_execution_unlock();
        return -1;
    }

    if (s_config_path[0] == '\0' || s_pipeline_step_count < 1) {
        snprintf(s_status_line, sizeof s_status_line, "Set config and generator paths first");
        desktop_execution_unlock();
        return -1;
    }

    n = s_pipeline_step_count;
    for (pi = 0; pi < n; pi++) {
        FILE *probe = fopen(s_pipeline[pi], "rb");

        if (probe == NULL) {
            snprintf(
                s_status_line,
                sizeof s_status_line,
                "Generator not found: %s",
                s_pipeline[pi]
            );
            desktop_execution_unlock();
            return -1;
        }

        fclose(probe);
    }

    job = (desktop_job_payload *) malloc(sizeof(*job));
    if (job == NULL) {
        snprintf(s_status_line, sizeof s_status_line, "Out of memory starting job");
        desktop_execution_unlock();
        return -1;
    }

    memset(job, 0, sizeof(*job));
    snprintf(job->config_path, sizeof job->config_path, "%s", s_config_path);
    job->step_count = n;
    for (pi = 0; pi < n; pi++) {
        snprintf(job->paths[pi], sizeof job->paths[pi], "%s", s_pipeline[pi]);
    }

    s_running = 1;
    if (n > 1) {
        snprintf(s_status_line, sizeof s_status_line, "Running pipeline (%d steps)…", n);
    } else {
        snprintf(s_status_line, sizeof s_status_line, "%s", "Running generator…");
    }

#if defined(_WIN32)
    s_thread_handle = (HANDLE) _beginthreadex(NULL, 0, desktop_worker_thread, job, 0, NULL);
    if (s_thread_handle == NULL) {
        s_running = 0;
        snprintf(s_status_line, sizeof s_status_line, "Failed to start worker thread");
        free(job);
        desktop_execution_unlock();
        return -1;
    }
#else
    if (pthread_create(&s_thread, NULL, desktop_worker_thread, job) != 0) {
        s_running = 0;
        snprintf(s_status_line, sizeof s_status_line, "Failed to start worker thread");
        free(job);
        desktop_execution_unlock();
        return -1;
    }

    s_pthread_joinable = 1;
#endif

    desktop_execution_unlock();
    return 0;
}

int desktop_execution_is_running(void) {
    int r;

    desktop_execution_lock();
    r = s_running;
    desktop_execution_unlock();
    return r;
}

const char *desktop_execution_status_line(void) {
    return s_status_line;
}

int desktop_execution_consume_completed_pulse(void) {
    int out;

    desktop_execution_lock();
    out = s_completed_pulse;
    s_completed_pulse = 0;
    desktop_execution_unlock();
    return out;
}
