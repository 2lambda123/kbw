use crate::bitwise::*;
use itertools::Itertools;
use num::complex::Complex64;
use rand::distributions::WeightedIndex;
use rand::prelude::*;
use rayon::prelude::*;
use std::{collections::HashMap, f64::consts::FRAC_1_SQRT_2};
use twox_hash::RandomXxHashBuilder64;

type StateMap = HashMap<Vec<u64>, Complex64, RandomXxHashBuilder64>;

pub struct Sparse {
    num_states: usize,
    state_0: StateMap,
    state_1: StateMap,
    state: bool,
}

impl Sparse {
    pub fn new() -> Sparse {
        Sparse {
            num_states: 0,
            state_0: Default::default(),
            state_1: Default::default(),
            state: true,
        }
    }

    fn get_states(&mut self) -> (&mut StateMap, &mut StateMap) {
        self.state = !self.state;
        if self.state {
            (&mut self.state_1, &mut self.state_0)
        } else {
            (&mut self.state_0, &mut self.state_1)
        }
    }

    fn get_current_state(&mut self) -> &mut StateMap {
        if self.state {
            &mut self.state_0
        } else {
            &mut self.state_1
        }
    }
}

impl crate::QuantumExecution for Sparse {
    fn prepare_for_execution(&mut self, metrics: &ket::Metrics) -> Result<(), String> {
        self.num_states = ((metrics.max_num_qubit + 64) / 64) as usize;

        let mut zero = Vec::new();
        zero.resize(self.num_states, 0u64);
        self.state_0.insert(zero, Complex64::new(1.0, 0.0));
        self.state = true;

        Ok(())
    }

    fn pauli_x(&mut self, target: u32, control: &[u32]) {
        let (current_state, next_state) = self.get_states();

        current_state.drain().for_each(|(state, amp)| {
            next_state.insert(
                if ctrl_check_vec(&state, control) {
                    bit_flip_vec(state, target)
                } else {
                    state
                },
                amp,
            );
        });
    }

    fn pauli_y(&mut self, target: u32, control: &[u32]) {
        let (current_state, next_state) = self.get_states();

        current_state.drain().for_each(|(state, mut amp)| {
            if ctrl_check_vec(&state, control) {
                amp *= if is_one_at_vec(&state, target) {
                    -Complex64::i()
                } else {
                    Complex64::i()
                };
                next_state.insert(bit_flip_vec(state, target), amp);
            } else {
                next_state.insert(state, amp);
            }
        });
    }

    fn pauli_z(&mut self, target: u32, control: &[u32]) {
        let current_state = self.get_current_state();

        current_state.par_iter_mut().for_each(|(state, amp)| {
            if ctrl_check_vec(state, control) && is_one_at_vec(state, target) {
                *amp = -*amp;
            }
        });
    }

    fn hadamard(&mut self, target: u32, control: &[u32]) {
        let (current_state, next_state) = self.get_states();

        current_state.drain().for_each(|(state, mut amp)| {
            if ctrl_check_vec(&state, control) {
                amp *= FRAC_1_SQRT_2;
                let state_flipped = bit_flip_vec(Vec::clone(&state), target);

                match next_state.get_mut(&state_flipped) {
                    Some(c_amp) => {
                        *c_amp += amp;
                        if c_amp.l1_norm() < 1e-15 {
                            next_state.remove(&state_flipped);
                        }
                    }
                    None => {
                        next_state.insert(state_flipped, amp);
                    }
                }

                amp = if is_one_at_vec(&state, target) {
                    -amp
                } else {
                    amp
                };

                match next_state.get_mut(&state) {
                    Some(c_amp) => {
                        *c_amp += amp;
                        if c_amp.l1_norm() < 1e-15 {
                            next_state.remove(&state);
                        }
                    }
                    None => {
                        next_state.insert(state, amp);
                    }
                }
            } else {
                next_state.insert(state, amp);
            }
        });
    }

    fn phase(&mut self, lambda: f64, target: u32, control: &[u32]) {
        let current_state = self.get_current_state();

        let phase = Complex64::exp(lambda * Complex64::i());

        current_state.par_iter_mut().for_each(|(state, amp)| {
            if ctrl_check_vec(state, control) && is_one_at_vec(state, target) {
                *amp *= phase;
            }
        });
    }

    fn rx(&mut self, theta: f64, target: u32, control: &[u32]) {
        let (current_state, next_state) = self.get_states();

        let cons_theta_2 = Complex64::from(f64::cos(theta / 2.0));
        let sin_theta_2 = -Complex64::i() * f64::sin(theta / 2.0);

        current_state.drain().for_each(|(state, amp)| {
            if ctrl_check_vec(&state, control) {
                let state_flipped = bit_flip_vec(Vec::clone(&state), target);

                match next_state.get_mut(&state_flipped) {
                    Some(c_amp) => {
                        *c_amp += amp * sin_theta_2;
                        if c_amp.l1_norm() < 1e-15 {
                            next_state.remove(&state_flipped);
                        }
                    }
                    None => {
                        next_state.insert(state_flipped, amp * sin_theta_2);
                    }
                }

                match next_state.get_mut(&state) {
                    Some(c_amp) => {
                        *c_amp += amp * cons_theta_2;
                        if c_amp.l1_norm() < 1e-15 {
                            next_state.remove(&state);
                        }
                    }
                    None => {
                        next_state.insert(state, amp * cons_theta_2);
                    }
                }
            } else {
                next_state.insert(state, amp);
            }
        });
    }

    fn ry(&mut self, theta: f64, target: u32, control: &[u32]) {
        let (current_state, next_state) = self.get_states();

        let cons_theta_2 = Complex64::from(f64::cos(theta / 2.0));
        let p_sin_theta_2 = -Complex64::i() * f64::sin(theta / 2.0);
        let m_sin_theta_2 = -p_sin_theta_2;

        current_state.drain().for_each(|(state, amp)| {
            if ctrl_check_vec(&state, control) {
                let state_flipped = bit_flip_vec(Vec::clone(&state), target);
                let flipped_amp = amp
                    * if is_one_at_vec(&state, target) {
                        m_sin_theta_2
                    } else {
                        p_sin_theta_2
                    };

                match next_state.get_mut(&state_flipped) {
                    Some(c_amp) => {
                        *c_amp += flipped_amp;
                        if c_amp.l1_norm() < 1e-15 {
                            next_state.remove(&state_flipped);
                        }
                    }
                    None => {
                        next_state.insert(state_flipped, flipped_amp);
                    }
                }

                match next_state.get_mut(&state) {
                    Some(c_amp) => {
                        *c_amp += amp * cons_theta_2;
                        if c_amp.l1_norm() < 1e-15 {
                            next_state.remove(&state);
                        }
                    }
                    None => {
                        next_state.insert(state, amp * cons_theta_2);
                    }
                }
            } else {
                next_state.insert(state, amp);
            }
        });
    }

    fn rz(&mut self, theta: f64, target: u32, control: &[u32]) {
        let current_state = self.get_current_state();

        let phase_0 = Complex64::exp(-theta / 2.0 * Complex64::i());
        let phase_1 = Complex64::exp(theta / 2.0 * Complex64::i());

        current_state.par_iter_mut().for_each(|(state, amp)| {
            if ctrl_check_vec(state, control) {
                if is_one_at_vec(state, target) {
                    *amp *= phase_1;
                } else {
                    *amp *= phase_0;
                }
            }
        });
    }

    fn measure(&mut self, target: u32) -> bool {
        let (current_state, next_state) = self.get_states();

        let p1: f64 = current_state
            .iter()
            .map(|(state, amp)| {
                if is_one_at_vec(state, target) {
                    amp.l1_norm().powi(2)
                } else {
                    0.0
                }
            })
            .sum();

        let p0 = match 1.0 - p1 {
            p0 if p0 >= 0.0 => p0,
            _ => 0.0,
        };

        let result = WeightedIndex::new([p0, p1])
            .unwrap()
            .sample(&mut thread_rng())
            == 1;

        let p = 1.0 / f64::sqrt(if result { p1 } else { p0 });

        current_state.drain().for_each(|(state, amp)| {
            if is_one_at_vec(&state, target) == result {
                next_state.insert(state, amp * p);
            }
        });

        result
    }

    fn dump(&mut self, qubits: &[u32]) -> ket::DumpData {
        let mut basis_states = Vec::new();
        let mut amplitudes_real = Vec::new();
        let mut amplitudes_img = Vec::new();

        let state = self.get_current_state();

        state.iter().for_each(|(state, amp)| {
            let mut state: Vec<u64> = qubits
                .iter()
                .rev()
                .chunks(64)
                .into_iter()
                .map(|qubits| {
                    qubits
                        .into_iter()
                        .enumerate()
                        .map(|(index, qubit)| (is_one_at_vec(state, *qubit) as usize) << index)
                        .reduce(|a, b| a | b)
                        .unwrap_or(0) as u64
                })
                .collect();
            state.reverse();

            basis_states.push(state);
            amplitudes_real.push(amp.re);
            amplitudes_img.push(amp.im);
        });

        ket::DumpData {
            basis_states,
            amplitudes_real,
            amplitudes_img,
        }
    }
}

#[cfg(test)]
mod tests {
    use ket::*;

    fn bell() -> (Process, Qubit, Qubit) {
        let mut p = Process::new(0);
        let a = p.alloc(false).unwrap();
        let b = p.alloc(false).unwrap();
        p.apply_gate(QuantumGate::Hadamard, &a).unwrap();
        p.ctrl_push(&[&a]).unwrap();
        p.apply_gate(QuantumGate::PauliX, &b).unwrap();
        p.ctrl_pop().unwrap();
        (p, a, b)
    }

    #[test]
    fn dump_bell() {
        let (mut p, a, b) = bell();
        let d = p.dump(&[&a, &b]).unwrap();
        crate::run_sparse_from_process(&mut p).unwrap();
        assert!(d.value().is_some());
        println!("{:?}", d);
    }

    #[test]
    fn measure_bell() {
        for _ in 0..10 {
            let (mut p, mut a, mut b) = bell();
            let m = p.measure(&mut [&mut a, &mut b]).unwrap();
            crate::run_sparse_from_process(&mut p).unwrap();
            let m = m.value().unwrap();
            assert!(m == 0 || m == 3);
        }
    }

    #[test]
    fn dump_h_3() {
        let mut p = Process::new(0);
        let q: Vec<Qubit> = (0..3)
            .into_iter()
            .map(|_| p.alloc(false).unwrap())
            .collect();

        q.iter()
            .for_each(|q| p.apply_gate(QuantumGate::Hadamard, q).unwrap());

        let q: Vec<&Qubit> = q.iter().collect();
        let d = p.dump(&q).unwrap();
        crate::run_sparse_from_process(&mut p).unwrap();
        assert!(d.value().is_some());
        println!("{:?}", d);
    }

    #[test]
    fn measure_hzh_10() {
        let mut p = Process::new(0);
        let mut q: Vec<Qubit> = (0..10)
            .into_iter()
            .map(|_| p.alloc(false).unwrap())
            .collect();

        q.iter()
            .for_each(|q| p.apply_gate(QuantumGate::Hadamard, q).unwrap());

        q.iter()
            .for_each(|q| p.apply_gate(QuantumGate::PauliZ, q).unwrap());

        q.iter()
            .for_each(|q| p.apply_gate(QuantumGate::Hadamard, q).unwrap());

        let mut q: Vec<&mut Qubit> = q.iter_mut().collect();
        let m = p.measure(&mut q).unwrap();
        crate::run_sparse_from_process(&mut p).unwrap();
        assert!(m.value().unwrap() == ((1 << 10) - 1));
        println!("{:?}; Execution Time = {}", m, p.exec_time().unwrap());
    }

    #[test]
    fn dump_ghz_100() {
        let mut p = Process::new(0);
        let q: Vec<Qubit> = (0..100)
            .into_iter()
            .map(|_| p.alloc(false).unwrap())
            .collect();

        p.apply_gate(QuantumGate::Hadamard, &q[0]).unwrap();

        p.ctrl_push(&[&q[0]]).unwrap();

        q.iter()
            .skip(1)
            .for_each(|q| p.apply_gate(QuantumGate::PauliX, q).unwrap());

        p.ctrl_pop().unwrap();

        let q: Vec<&Qubit> = q.iter().collect();
        let d = p.dump(&q).unwrap();
        crate::run_sparse_from_process(&mut p).unwrap();
        println!("{:?}; Execution Time = {}", d, p.exec_time().unwrap());
    }
}
