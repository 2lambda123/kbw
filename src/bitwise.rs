use num::Integer;

pub fn bit_flip(state: usize, index: u32) -> usize {
    state ^ (1 << index)
}

pub fn bit_flip_vec(mut state: Vec<u64>, index: u32) -> Vec<u64> {
    let (outer_index, inner_index) = index.div_mod_floor(&64);
    state[outer_index as usize] =
        bit_flip(state[outer_index as usize] as usize, inner_index) as u64;
    state
}

pub fn is_one_at(state: usize, target: u32) -> bool {
    state & (1 << target) != 0
}

pub fn is_one_at_vec(state: &[u64], target: u32) -> bool {
    let (outer_index, inner_index) = target.div_mod_floor(&64);
    state[outer_index as usize] & (1 << inner_index) != 0
}

pub fn ctrl_check(state: usize, control: &[u32]) -> bool {
    control.iter().all(|control| is_one_at(state, *control))
}

pub fn ctrl_check_vec(state: &[u64], control: &[u32]) -> bool {
    control.iter().all(|control| is_one_at_vec(state, *control))
}
