use std::{error::Error, fmt::Display, result};

use num::{FromPrimitive, ToPrimitive};
use num_derive::{FromPrimitive, ToPrimitive};

#[derive(Debug, FromPrimitive, ToPrimitive)]
pub enum KBWError {
    Success,
    Timeout,
    OutOfQubits,
    UnsupportedNumberOfQubits,
    UnsupportedPlugin,
    NotReadyForExecution,
    UndefinedDataType,
    UndefinedSimMode,
    UndefinedError,
}

pub type Result<T> = result::Result<T, KBWError>;

impl KBWError {
    pub fn to_str(&self) -> &'static str {
        match self {
            KBWError::Success => "the call returned successfully",
            KBWError::UndefinedError => "undefined error",
            KBWError::Timeout => "quantum execution timeout",
            KBWError::OutOfQubits => {
                "cannot allocate more qubits (maybe you are deallocating too many qubits as dirty)"
            }
            KBWError::UnsupportedNumberOfQubits => {
                "dense simulator do not allow more then 32 qubits"
            }
            KBWError::UnsupportedPlugin => "unsupported plugin gate",
            KBWError::NotReadyForExecution => "not ready for execution",
            KBWError::UndefinedSimMode => "undefined simulation mode",
            KBWError::UndefinedDataType => "undefined data type",
        }
    }

    pub fn error_code(&self) -> i32 {
        self.to_i32().unwrap()
    }

    pub fn from_error_code(error_code: i32) -> KBWError {
        Self::from_i32(error_code).unwrap_or(KBWError::UndefinedError)
    }
}

impl Display for KBWError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self.to_str())
    }
}

impl Error for KBWError {}

#[cfg(test)]
mod tests {
    use super::KBWError;

    #[test]
    fn success_is_zero() {
        assert!(KBWError::Success.error_code() == 0)
    }

    #[test]
    fn print_error_code() {
        let mut error_code = 0;
        loop {
            let error = KBWError::from_error_code(error_code);
            println!("#define KBW_{:#?} {}", error, error_code);

            if let KBWError::UndefinedError = error {
                break;
            } else {
                error_code += 1;
            }
        }
    }
}
