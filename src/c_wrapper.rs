static mut ERROR_MESSAGE: String = String::new();

#[no_mangle]
pub extern "C" fn kbw_error_message(size: &mut usize) -> *const u8 {
    *size = unsafe { ERROR_MESSAGE.len() };
    unsafe { ERROR_MESSAGE.as_ptr() }
}

pub enum KBWResult {
    JSON(String),
    BIN(Vec<u8>),
}

#[no_mangle]
pub extern "C" fn kbw_result_get(result: &KBWResult, size: &mut usize) -> *const u8 {
    match result {
        KBWResult::JSON(json) => {
            *size = json.len();
            json.as_ptr()
        }
        KBWResult::BIN(bin) => {
            *size = bin.len();
            bin.as_ptr()
        }
    }
}

#[no_mangle]
pub extern "C" fn kbw_result_delete(result: *mut KBWResult) {
    unsafe { Box::from_raw(result) };
}

#[no_mangle]
pub extern "C" fn kbw_run_dense_from_json(
    quantum_code: *const u8,
    quantum_code_size: usize,
    metrics: *const u8,
    metrics_size: usize,
) -> *const KBWResult {
    let quantum_code = unsafe { std::slice::from_raw_parts(quantum_code, quantum_code_size) };
    let quantum_code = match std::str::from_utf8(quantum_code) {
        Ok(json) => json,
        Err(msg) => {
            unsafe { ERROR_MESSAGE = format!("Fail to load quantum code: {}", msg) };
            return 0 as *const KBWResult;
        }
    };

    let metrics = unsafe { std::slice::from_raw_parts(metrics, metrics_size) };
    let metrics = match std::str::from_utf8(metrics) {
        Ok(json) => json,
        Err(msg) => {
            unsafe { ERROR_MESSAGE = format!("Fail to load metrics: {}", msg) };
            return 0 as *const KBWResult;
        }
    };

    match crate::run_dense_from_json(quantum_code, metrics) {
        Ok(result) => Box::into_raw(Box::new(KBWResult::JSON(result))),
        Err(msg) => {
            unsafe { ERROR_MESSAGE = msg };
            0 as *const KBWResult
        }
    }
}

#[no_mangle]
pub extern "C" fn kbw_run_dense_from_bin(
    quantum_code: *const u8,
    quantum_code_size: usize,
    metrics: *const u8,
    metrics_size: usize,
) -> *const KBWResult {
    let quantum_code = unsafe { std::slice::from_raw_parts(quantum_code, quantum_code_size) };
    let metrics = unsafe { std::slice::from_raw_parts(metrics, metrics_size) };

    match crate::run_dense_from_bin(quantum_code, metrics) {
        Ok(result) => Box::into_raw(Box::new(KBWResult::BIN(result))),
        Err(msg) => {
            unsafe { ERROR_MESSAGE = msg };
            0 as *const KBWResult
        }
    }
}

#[no_mangle]
pub extern "C" fn kbw_run_dense_from_process(process: &mut ket::Process) -> i32 {
    match crate::run_dense_from_process(process) {
        Ok(_) => 0,
        Err(msg) => {
            unsafe { ERROR_MESSAGE = msg };
            1
        }
    }
}

#[no_mangle]
pub extern "C" fn kbw_run_sparse_from_json(
    quantum_code: *const u8,
    quantum_code_size: usize,
    metrics: *const u8,
    metrics_size: usize,
) -> *const KBWResult {
    let quantum_code = unsafe { std::slice::from_raw_parts(quantum_code, quantum_code_size) };
    let quantum_code = match std::str::from_utf8(quantum_code) {
        Ok(json) => json,
        Err(msg) => {
            unsafe { ERROR_MESSAGE = format!("Fail to load quantum code: {}", msg) };
            return 0 as *const KBWResult;
        }
    };

    let metrics = unsafe { std::slice::from_raw_parts(metrics, metrics_size) };
    let metrics = match std::str::from_utf8(metrics) {
        Ok(json) => json,
        Err(msg) => {
            unsafe { ERROR_MESSAGE = format!("Fail to load metrics: {}", msg) };
            return 0 as *const KBWResult;
        }
    };

    match crate::run_sparse_from_json(quantum_code, metrics) {
        Ok(result) => Box::into_raw(Box::new(KBWResult::JSON(result))),
        Err(msg) => {
            unsafe { ERROR_MESSAGE = msg };
            0 as *const KBWResult
        }
    }
}

#[no_mangle]
pub extern "C" fn kbw_run_sparse_from_bin(
    quantum_code: *const u8,
    quantum_code_size: usize,
    metrics: *const u8,
    metrics_size: usize,
) -> *const KBWResult {
    let quantum_code = unsafe { std::slice::from_raw_parts(quantum_code, quantum_code_size) };
    let metrics = unsafe { std::slice::from_raw_parts(metrics, metrics_size) };

    match crate::run_sparse_from_bin(quantum_code, metrics) {
        Ok(result) => Box::into_raw(Box::new(KBWResult::BIN(result))),
        Err(msg) => {
            unsafe { ERROR_MESSAGE = msg };
            0 as *const KBWResult
        }
    }
}

#[no_mangle]
pub extern "C" fn kbw_run_sparse_from_process(process: &mut ket::Process) -> i32 {
    match crate::run_sparse_from_process(process) {
        Ok(_) => 0,
        Err(msg) => {
            unsafe { ERROR_MESSAGE = msg };
            1
        }
    }
}
