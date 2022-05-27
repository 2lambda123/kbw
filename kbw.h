#pragma once
#include <stdint.h>
#include <stddef.h>

typedef void *kbw_result_t;

const uint8_t *kbw_error_message(size_t *size);

void kbw_result_delete(kbw_result_t result);
const uint8_t *kbw_result_get(kbw_result_t result, size_t *size);

kbw_result_t kbw_run_dense_from_json(const uint8_t *quantum_code, size_t quantum_code_size, const uint8_t *metrics, size_t metrics_size);
kbw_result_t kbw_run_dense_from_bin(const uint8_t *quantum_code, size_t quantum_code_size, const uint8_t *metrics, size_t metrics_size);
int32_t kbw_run_dense_from_process(void *process);
