#include <string.h>

#include "beast2/runtime.h"
#include "test_support.h"

static int test_diffusion_runtime_and_cache(void) {
    beast2_model_runtime_context context;
    beast2_model_request request;
    beast2_model_handle first_handle;
    beast2_model_handle second_handle;
    beast2_model_result result;
    beast2_model_result second_result;
    beast2_tensor_memory_telemetry telemetry;
    char error_message[512];

    memset(&request, 0, sizeof(request));
    memset(&second_result, 0, sizeof(second_result));
    memset(&telemetry, 0, sizeof(telemetry));
    beast2_model_runtime_init(&context);

    request.engine = "sdxl";
    request.checkpoint = "sdxl#cache001";
    request.prompt = "Subject: cache test portrait";
    request.seed = "17";
    request.steps = "4";
    request.resolution = "4x4";
    request.backend = "onnx";
    request.precision = "fp32";

    BEAST2_TEST_ASSERT(
        beast2_model_load(&context, &request, &first_handle, error_message, sizeof(error_message)) == 0
    );
    BEAST2_TEST_ASSERT(first_handle.category == BEAST2_MODEL_CATEGORY_DIFFUSION);
    BEAST2_TEST_ASSERT(first_handle.backend == BEAST2_RUNTIME_BACKEND_ONNX);
    BEAST2_TEST_ASSERT(first_handle.precision == BEAST2_PRECISION_FP32);
    BEAST2_TEST_ASSERT(first_handle.cache_hit == 0);

    BEAST2_TEST_ASSERT(
        beast2_model_load(&context, &request, &second_handle, error_message, sizeof(error_message)) == 0
    );
    BEAST2_TEST_ASSERT(second_handle.cache_hit == 1);
    BEAST2_TEST_ASSERT(context.cache_hits == 1);
    BEAST2_TEST_ASSERT(context.cache_misses == 1);

    BEAST2_TEST_ASSERT(
        beast2_model_infer(&context, &first_handle, &request, &result, error_message, sizeof(error_message)) == 0
    );
    BEAST2_TEST_ASSERT(result.output_kind == BEAST2_OUTPUT_KIND_IMAGE);
    BEAST2_TEST_ASSERT_STRING_EQ(result.file_extension, "ppm");
    BEAST2_TEST_ASSERT(strstr(result.content, "P3\n") == result.content);
    BEAST2_TEST_ASSERT(strstr(result.content, "# engine sdxl") != NULL);
    BEAST2_TEST_ASSERT(strstr(result.content, "4 4\n255\n") != NULL);
    BEAST2_TEST_ASSERT(result.tensor_pool_misses > 0);

    BEAST2_TEST_ASSERT(
        beast2_model_infer(&context, &second_handle, &request, &second_result, error_message, sizeof(error_message)) == 0
    );
    BEAST2_TEST_ASSERT(second_result.tensor_pool_hits > 0);
    BEAST2_TEST_ASSERT(second_result.tensor_bytes_reused_cpu > 0);
    BEAST2_TEST_ASSERT(second_result.tensor_bytes_reused_gpu > 0);

    beast2_model_runtime_get_tensor_telemetry(&context, &telemetry);
    BEAST2_TEST_ASSERT(telemetry.pool_hits > 0);
    BEAST2_TEST_ASSERT(telemetry.pool_misses > 0);
    BEAST2_TEST_ASSERT(telemetry.peak_reserved_cpu > 0);
    BEAST2_TEST_ASSERT(telemetry.peak_reserved_gpu > 0);

    beast2_model_unload(&context, &second_handle);
    beast2_model_unload(&context, &first_handle);
    beast2_model_runtime_shutdown(&context);
    return 0;
}

static int test_llm_runtime_output(void) {
    beast2_model_runtime_context context;
    beast2_model_request request;
    beast2_model_handle handle;
    beast2_model_result result;
    char error_message[512];

    memset(&request, 0, sizeof(request));
    beast2_model_runtime_init(&context);

    request.engine = "qwen";
    request.checkpoint = "qwen#demo001";
    request.prompt = "Generate a short caption";
    request.seed = "99";
    request.steps = "12";

    BEAST2_TEST_ASSERT(
        beast2_model_load(&context, &request, &handle, error_message, sizeof(error_message)) == 0
    );
    BEAST2_TEST_ASSERT(handle.category == BEAST2_MODEL_CATEGORY_LLM);
    BEAST2_TEST_ASSERT(handle.backend == BEAST2_RUNTIME_BACKEND_LLAMA_CPP);

    BEAST2_TEST_ASSERT(
        beast2_model_infer(&context, &handle, &request, &result, error_message, sizeof(error_message)) == 0
    );
    BEAST2_TEST_ASSERT(result.output_kind == BEAST2_OUTPUT_KIND_TEXT);
    BEAST2_TEST_ASSERT_STRING_EQ(result.file_extension, "txt");
    BEAST2_TEST_ASSERT(strstr(result.content, "BEAST2_PHASE3_LLM_OUTPUT") != NULL);
    BEAST2_TEST_ASSERT(strstr(result.content, "Generate a short caption") != NULL);

    beast2_model_unload(&context, &handle);
    beast2_model_runtime_shutdown(&context);
    return 0;
}

int main(void) {
    if (test_diffusion_runtime_and_cache() != 0) {
        return 1;
    }

    if (test_llm_runtime_output() != 0) {
        return 1;
    }

    return 0;
}
