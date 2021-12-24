use crate::kmp::calc_next;
use std::cmp::*;

#[cfg(target_pointer_width = "64")]
const LARGE: usize = 10_000_000_000_000_000_000;

#[cfg(not(target_pointer_width = "64"))]
const LARGE: usize = 2_000_000_000;

pub struct BMPattern<'a> {
    pat_bytes: &'a [u8],
    delta_1: [usize; 256],
    delta_2: Vec<usize>,
    k: usize, // pat的最短周期长度
}

impl<'a> BMPattern<'a> {
    pub fn new(pat: &'a [u8]) -> BMPattern<'a> {
        let ans = build_delta_2(pat);
        BMPattern {
            pat_bytes: pat,
            delta_1: build_delta_1(pat),
            delta_2: ans.0,
            k: ans.1,
        }
    }
    // ...
    fn delta0(&self, c: u8) -> usize {
        if c == self.pat_bytes[self.pat_bytes.len() - 1] {
            return LARGE;
        }
        self.delta_1[c as usize]
    }
    pub fn find_all(&self, string_bytes: &[u8]) -> Vec<usize> {
        let mut result = vec![];
        let stringlen = string_bytes.len();
        let patlen = self.pat_bytes.len();
        let pat_last_pos = patlen - 1;
        let mut string_index = pat_last_pos;
        let mut pat_index;
        let l0 = patlen - self.k;
        let mut l = 0;

        while string_index < stringlen {
            let old_string_index = string_index;

            while string_index < stringlen {
                string_index += self.delta0(string_bytes[string_index]);
            }
            if string_index < LARGE {
                break;
            }

            string_index -= LARGE;

            // 如果string_index发生移动，意味着自从上次成功匹配后发生了至少一次的失败匹配。
            // 此时需要将Galil规则的二次匹配的偏移量归零。
            if old_string_index < string_index {
                l = 0;
            }

            pat_index = pat_last_pos;

            while pat_index > l && string_bytes[string_index] == self.pat_bytes[pat_index] {
                string_index -= 1;
                pat_index -= 1;
            }

            if pat_index == l && string_bytes[string_index] == self.pat_bytes[pat_index] {
                result.push(string_index - l);

                string_index += pat_last_pos - l + self.k;
                l = l0;
            } else {
                l = 0;
                string_index += max(
                    self.delta_1[string_bytes[string_index] as usize],
                    self.delta_2[pat_index],
                );
            }
        }

        result
    }
}

fn build_delta_1(p: &[u8]) -> [usize; 256] {
    let len = p.len();
    let mut ans: [usize; 256] = [len; 256];
    let lastpos = len - 1;
    for i in (0..len - 1).rev() {
        if ans[p[i] as usize] == len {
            ans[p[i] as usize] = lastpos - i;
        }
    }
    ans
}

fn build_delta_2(p: &[impl PartialEq]) -> (Vec<usize>, usize) {
    let patlen = p.len();
    let lastpos = patlen - 1;
    let mut delta_2 = Vec::with_capacity(patlen);

    // 第一种情况
    // delta_2[j] = lastpos * 2 - j
    for i in 0..patlen {
        delta_2.push(lastpos * 2 - i);
    }

    // 第二种情况
    // lastpos <= delata2[j] = lastpos * 2 - j
    let pi = calc_next(p);
    let k = pi[lastpos];
    let mut i = lastpos;
    let mut last_i = lastpos;
    while pi[i] > 0 {
        let start;
        let end;

        if i == lastpos {
            start = 0;
        } else {
            start = patlen - pi[last_i];
        }

        end = patlen - pi[i];

        for j in start..end {
            delta_2[j] = lastpos * 2 - j - pi[i];
        }

        last_i = i;
        i = pi[i] - 1;
    }

    // 第三种情况
    // delata2[j] < lastpos
    let mut j = lastpos;
    let mut t = patlen;
    let mut f = pi;
    loop {
        f[j] = t;
        while t < patlen && p[j] != p[t] {
            delta_2[t] = min(delta_2[t], lastpos - 1 - j); // 使用min函数保证后面可能的回退不会覆盖前面的数据
            t = f[t];
        }

        t -= 1;
        if j == 0 {
            break;
        }
        j -= 1;
    }
    delta_2[lastpos] = 0;

    (delta_2, patlen - k)
}

pub fn string_match(vpat: &[u8], vstr: &[u8]) -> Vec<usize> {
    let bmp = BMPattern::new(vpat);
    bmp.find_all(vstr)
}
