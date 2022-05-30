use ket::Instruction;
use std::time::Instant;

pub trait QuantumExecution {
    fn prepare_for_execution(&mut self, metrics: &ket::Metrics) -> Result<(), String>;
    fn pauli_x(&mut self, target: u32, control: &[u32]);
    fn pauli_y(&mut self, target: u32, control: &[u32]);
    fn pauli_z(&mut self, target: u32, control: &[u32]);
    fn hadamard(&mut self, target: u32, control: &[u32]);
    fn phase(&mut self, lambda: f64, target: u32, control: &[u32]);
    fn rx(&mut self, theta: f64, target: u32, control: &[u32]);
    fn ry(&mut self, theta: f64, target: u32, control: &[u32]);
    fn rz(&mut self, theta: f64, target: u32, control: &[u32]);
    fn measure(&mut self, target: u32) -> bool;
    fn dump(&mut self, qubits: &[u32]) -> ket::DumpData;
    fn plugin(
        &mut self,
        name: &str,
        target: &[u32],
        control: &[u32],
        adj: bool,
        args: &str,
    ) -> Result<(), String>;

    fn run(
        &mut self,
        quantum_code: &[&[Instruction]],
        metrics: ket::Metrics,
    ) -> Result<ket::QuantumResult, String> {
        let mut qubit_stack: Vec<u32> = (0..metrics.max_num_qubit).collect();
        let mut qubit_stack_dirty = Vec::new();
        let mut qubit_map: Vec<u32> = (0..metrics.max_num_qubit).collect();

        let mut int_register = Vec::with_capacity(metrics.future_count as usize);
        for _ in 0..metrics.future_count {
            int_register.push(0);
        }

        let mut dump_register = Vec::with_capacity(metrics.dump_count as usize);
        for _ in 0..metrics.dump_count {
            dump_register.push(ket::DumpData {
                basis_states: Vec::new(),
                amplitudes_img: Vec::new(),
                amplitudes_real: Vec::new(),
            });
        }

        self.prepare_for_execution(&metrics)?;

        let mut current_block = 0usize;

        let qubit_vec_map = |control: &[u32], qubit_map: &[u32]| -> Vec<u32> {
            control
                .iter()
                .map(|index| qubit_map[*index as usize])
                .collect()
        };

        let start = Instant::now();

        'quantum_run: loop {
            for instruction in quantum_code[current_block] {
                if let Some(timeout) = metrics.timeout {
                    if start.elapsed().as_secs() > timeout as u64 {
                        return Err(String::from("Quantum execution timeout."));
                    }
                }
                match instruction {
                    Instruction::Alloc { dirty, target } => {
                        let qubit_index = if *dirty & !qubit_stack_dirty.is_empty() {
                            qubit_stack_dirty.pop().unwrap()
                        } else {
                            match qubit_stack.pop() {
                                Some(index) => index,
                                None => return Err(String::from("Cannot allocate more qubits. Maybe you are deallocating too many qubits as dirty."))
                            }
                        };
                        qubit_map[*target as usize] = qubit_index;
                    }
                    Instruction::Free { dirty, target } => {
                        if *dirty {
                            qubit_stack_dirty.push(qubit_map[*target as usize]);
                        } else {
                            qubit_stack.push(qubit_map[*target as usize]);
                        }
                    }
                    Instruction::Gate {
                        gate,
                        target,
                        control,
                    } => match gate {
                        ket::QuantumGate::PauliX => self.pauli_x(
                            qubit_map[*target as usize],
                            &qubit_vec_map(control, &qubit_map),
                        ),
                        ket::QuantumGate::PauliY => self.pauli_y(
                            qubit_map[*target as usize],
                            &qubit_vec_map(control, &qubit_map),
                        ),
                        ket::QuantumGate::PauliZ => self.pauli_z(
                            qubit_map[*target as usize],
                            &qubit_vec_map(control, &qubit_map),
                        ),
                        ket::QuantumGate::Hadamard => self.hadamard(
                            qubit_map[*target as usize],
                            &qubit_vec_map(control, &qubit_map),
                        ),
                        ket::QuantumGate::Phase(lambda) => self.phase(
                            *lambda,
                            qubit_map[*target as usize],
                            &qubit_vec_map(control, &qubit_map),
                        ),
                        ket::QuantumGate::RX(theta) => self.rx(
                            *theta,
                            qubit_map[*target as usize],
                            &qubit_vec_map(control, &qubit_map),
                        ),
                        ket::QuantumGate::RY(theta) => self.ry(
                            *theta,
                            qubit_map[*target as usize],
                            &qubit_vec_map(control, &qubit_map),
                        ),
                        ket::QuantumGate::RZ(theta) => self.rz(
                            *theta,
                            qubit_map[*target as usize],
                            &qubit_vec_map(control, &qubit_map),
                        ),
                    },
                    Instruction::Measure { qubits, output } => {
                        int_register[*output as usize] = qubits
                            .iter()
                            .rev()
                            .map(|qubit| qubit_map[*qubit as usize])
                            .enumerate()
                            .map(|(index, qubit)| (self.measure(qubit) as i64) << index)
                            .reduce(|a, b| a | b)
                            .unwrap_or(0);
                    }
                    Instruction::Plugin {
                        name,
                        target,
                        control,
                        adj,
                        args,
                    } => {
                        self.plugin(
                            name,
                            &qubit_vec_map(target, &qubit_map),
                            &qubit_vec_map(control, &qubit_map),
                            *adj,
                            args,
                        )?;
                    }
                    Instruction::Jump { addr } => {
                        current_block = *addr as usize;
                    }
                    Instruction::Branch {
                        test,
                        then,
                        otherwise,
                    } => {
                        current_block = if int_register[*test as usize] != 0 {
                            *then as usize
                        } else {
                            *otherwise as usize
                        };
                    }
                    Instruction::IntOp {
                        op,
                        result,
                        lhs,
                        rhs,
                    } => match op {
                        ket::ClassicalOp::Eq => {
                            int_register[*result as usize] =
                                (int_register[*lhs as usize] == int_register[*rhs as usize]) as i64;
                        }
                        ket::ClassicalOp::Neq => {
                            int_register[*result as usize] =
                                (int_register[*lhs as usize] != int_register[*rhs as usize]) as i64;
                        }
                        ket::ClassicalOp::Gt => {
                            int_register[*result as usize] =
                                (int_register[*lhs as usize] > int_register[*rhs as usize]) as i64;
                        }
                        ket::ClassicalOp::Geq => {
                            int_register[*result as usize] =
                                (int_register[*lhs as usize] >= int_register[*rhs as usize]) as i64;
                        }
                        ket::ClassicalOp::Lt => {
                            int_register[*result as usize] =
                                (int_register[*lhs as usize] < int_register[*rhs as usize]) as i64;
                        }
                        ket::ClassicalOp::Leq => {
                            int_register[*result as usize] =
                                (int_register[*lhs as usize] <= int_register[*rhs as usize]) as i64;
                        }
                        ket::ClassicalOp::Add => {
                            int_register[*result as usize] =
                                int_register[*lhs as usize] + int_register[*rhs as usize];
                        }
                        ket::ClassicalOp::Sub => {
                            int_register[*result as usize] =
                                int_register[*lhs as usize] - int_register[*rhs as usize];
                        }
                        ket::ClassicalOp::Mul => {
                            int_register[*result as usize] =
                                int_register[*lhs as usize] * int_register[*rhs as usize];
                        }
                        ket::ClassicalOp::Div => {
                            int_register[*result as usize] =
                                int_register[*lhs as usize] / int_register[*rhs as usize];
                        }
                        ket::ClassicalOp::Sll => {
                            int_register[*result as usize] =
                                int_register[*lhs as usize] << int_register[*rhs as usize];
                        }
                        ket::ClassicalOp::Srl => {
                            int_register[*result as usize] =
                                int_register[*lhs as usize] >> int_register[*rhs as usize];
                        }
                        ket::ClassicalOp::And => {
                            int_register[*result as usize] =
                                int_register[*lhs as usize] & int_register[*rhs as usize];
                        }
                        ket::ClassicalOp::Or => {
                            int_register[*result as usize] =
                                int_register[*lhs as usize] | int_register[*rhs as usize];
                        }
                        ket::ClassicalOp::Xor => {
                            int_register[*result as usize] =
                                int_register[*lhs as usize] ^ int_register[*rhs as usize];
                        }
                    },
                    Instruction::IntSet { result, value } => {
                        int_register[*result as usize] = *value;
                    }
                    Instruction::Dump { qubits, output } => {
                        dump_register[*output as usize] =
                            self.dump(&qubit_vec_map(qubits, &qubit_map));
                    }
                    Instruction::Halt => break 'quantum_run,
                }
            }
        }

        let exec_time = start.elapsed().as_secs_f64();

        Ok(ket::QuantumResult {
            future: int_register,
            dump: dump_register,
            exec_time,
        })
    }

    fn run_from_process(
        &mut self,
        process: &mut ket::Process,
    ) -> Result<ket::QuantumResult, String> {
        let metrics = process.get_metrics();
        let quantum_code = process.get_quantum_code().unwrap();
        self.run(&quantum_code, metrics)
    }

    fn run_from_json(
        &mut self,
        quantum_code: &str,
        metrics: &str,
    ) -> Result<ket::QuantumResult, String> {
        let quantum_code: Vec<Vec<Instruction>> = serde_json::from_str(quantum_code).unwrap();
        let quantum_code: Vec<&[Instruction]> = quantum_code.iter().map(|inst| &inst[..]).collect();
        let metrics: ket::Metrics = serde_json::from_str(metrics).unwrap();
        self.run(&quantum_code, metrics)
    }

    fn run_from_bin(
        &mut self,
        quantum_code: &[u8],
        metrics: &[u8],
    ) -> Result<ket::QuantumResult, String> {
        let quantum_code: Vec<Vec<Instruction>> = bincode::deserialize(quantum_code).unwrap();
        let quantum_code: Vec<&[Instruction]> = quantum_code.iter().map(|inst| &inst[..]).collect();
        let metrics: ket::Metrics = bincode::deserialize(metrics).unwrap();
        self.run(&quantum_code, metrics)
    }
}
