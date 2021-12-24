use std::cmp::PartialEq;

pub fn calc_next(cstr: &[impl PartialEq]) -> Vec<usize> {
    let mut ans: Vec<usize> = Vec::with_capacity(cstr.len());
    ans.push(0);
    for i in 1..cstr.len() {
        let mut j = ans[i - 1];
        while j > 0 && cstr[i] != cstr[j] {
            j = ans[j - 1];
        }
        if cstr[i] == cstr[j] {
            j += 1;
        }
        ans.push(j);
    }
    ans
}

pub fn string_match<T: PartialEq>(vpat: &[T], vstr: &[T]) -> Vec<usize> {
    let mut ans: Vec<usize> = Vec::new();
    let next = calc_next(vpat);
    let mut j: usize = 0;
    for i in 0..vstr.len() {
        while j > 0 && vpat[j] != vstr[i] {
            j = next[j - 1];
        }
        if vpat[j] == vstr[i] {
            j += 1;
        }
        if j == vpat.len() {
            ans.push(i + 1 - j);
            j = next[j - 1];
        }
    }
    ans
}
