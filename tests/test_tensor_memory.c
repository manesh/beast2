#include <string.h>

#include "beast2/tensor_memory.h"
#include "test_support.h"

static int test_cpu_and_gpu_pool_reuse(void) {
    beast2_tensor_memory_context context;
    beast2_tensor_buffer first_cpu;
    beast2_tensor_buffer second_cpu;
    beast2_tensor_buffer first_gpu;
    beast2_tensor_buffer second_gpu;
    beast2_tensor_memory_telemetry telemetry;
    char error_message[512];
    beast2_tensor_desc cpu_desc;
    beast2_tensor_desc gpu_desc;

    memset(&first_cpu, 0, sizeof(first_cpu));
    memset(&second_cpu, 0, sizeof(second_cpu));
    memset(&first_gpu, 0, sizeof(first_gpu));
    memset(&second_gpu, 0, sizeof(second_gpu));
    memset(&telemetry, 0, sizeof(telemetry));
    memset(&cpu_desc, 0, sizeof(cpu_desc));
    memset(&gpu_desc, 0, sizeof(gpu_desc));

    beast2_tensor_memory_init(&context);

    cpu_desc.device = BEAST2_TENSOR_DEVICE_CPU;
    cpu_desc.dtype = BEAST2_TENSOR_DTYPE_UINT8;
    cpu_desc.kind = BEAST2_TENSOR_KIND_IMAGE;
    cpu_desc.width = 8;
    cpu_desc.height = 8;
    cpu_desc.channels = 3;

    gpu_desc.device = BEAST2_TENSOR_DEVICE_GPU;
    gpu_desc.dtype = BEAST2_TENSOR_DTYPE_FP16;
    gpu_desc.kind = BEAST2_TENSOR_KIND_LATENT;
    gpu_desc.width = 4;
    gpu_desc.height = 4;
    gpu_desc.channels = 4;

    BEAST2_TEST_ASSERT(
        beast2_tensor_memory_acquire(&context, &cpu_desc, &first_cpu, error_message, sizeof(error_message)) == 0
    );
    BEAST2_TEST_ASSERT(first_cpu.reused == 0);
    BEAST2_TEST_ASSERT(first_cpu.size_bytes == 8 * 8 * 3);
    beast2_tensor_memory_release(&context, &first_cpu);

    BEAST2_TEST_ASSERT(
        beast2_tensor_memory_acquire(&context, &cpu_desc, &second_cpu, error_message, sizeof(error_message)) == 0
    );
    BEAST2_TEST_ASSERT(second_cpu.reused == 1);
    beast2_tensor_memory_release(&context, &second_cpu);

    BEAST2_TEST_ASSERT(
        beast2_tensor_memory_acquire(&context, &gpu_desc, &first_gpu, error_message, sizeof(error_message)) == 0
    );
    BEAST2_TEST_ASSERT(first_gpu.reused == 0);
    beast2_tensor_memory_release(&context, &first_gpu);

    BEAST2_TEST_ASSERT(
        beast2_tensor_memory_acquire(&context, &gpu_desc, &second_gpu, error_message, sizeof(error_message)) == 0
    );
    BEAST2_TEST_ASSERT(second_gpu.reused == 1);
    beast2_tensor_memory_release(&context, &second_gpu);

    beast2_tensor_memory_get_telemetry(&context, &telemetry);
    BEAST2_TEST_ASSERT(telemetry.pool_hits == 2);
    BEAST2_TEST_ASSERT(telemetry.pool_misses == 2);
    BEAST2_TEST_ASSERT(telemetry.cpu_pool_hits == 1);
    BEAST2_TEST_ASSERT(telemetry.gpu_pool_hits == 1);
    BEAST2_TEST_ASSERT(telemetry.bytes_reused_cpu > 0);
    BEAST2_TEST_ASSERT(telemetry.bytes_reused_gpu > 0);
    BEAST2_TEST_ASSERT(telemetry.peak_reserved_cpu > 0);
    BEAST2_TEST_ASSERT(telemetry.peak_reserved_gpu > 0);

    beast2_tensor_memory_shutdown(&context);
    return 0;
}

int main(void) {
    return test_cpu_and_gpu_pool_reuse();
}
