// mod bloom_filter;
mod bloom_filter;
mod bm;
mod kmp;

// use chrono;
use chrono::prelude::*;
use mt19937;
// use rand_core::RngCore;
use rand_core::RngCore;

fn calc_time(
    n: usize,
    len: (usize, usize),
    str_gen: fn(eng: &mut dyn rand_core::RngCore, len: (usize, usize)) -> (Vec<u8>, Vec<u8>), // (string, pattern)
) -> (f64, f64) {
    let mut rd = mt19937::MT19937::new_with_slice_seed(&[Utc::now().nanosecond()]);
    let mut time_start: i64;
    let mut times_kmp: i64 = 0;
    let mut times_bm: i64 = 0;
    let mut ans_kmp;
    let mut ans_bm;
    for _ in 0..n {
        let (vstr, vpat) = str_gen(&mut rd, len);
        {
            time_start = chrono::Local::now().timestamp_nanos();
            ans_kmp = kmp::string_match(&vpat, &vstr);
            times_kmp += chrono::Local::now().timestamp_nanos() - time_start;
        }
        {
            time_start = chrono::Local::now().timestamp_nanos();
            ans_bm = bm::string_match(&vpat, &vstr);
            times_bm += chrono::Local::now().timestamp_nanos() - time_start;
        }
        assert_eq!(ans_kmp, ans_bm);
    }
    (times_kmp as f64 / n as f64, times_bm as f64 / n as f64)
}

fn gen_rand_01(eng: &mut dyn rand_core::RngCore, len: (usize, usize)) -> (Vec<u8>, Vec<u8>) {
    let str_len = len.0;
    let pat_len = len.1;
    let mut vstr: Vec<u8> = Vec::with_capacity(str_len);
    let mut vpat: Vec<u8> = Vec::with_capacity(pat_len);
    for _ in 0..str_len {
        vstr.push((eng.next_u32() % 2) as u8);
    }
    for _ in 0..pat_len {
        vpat.push((eng.next_u32() % 2) as u8);
    }
    (vstr, vpat)
}

fn gen_rand_4(eng: &mut dyn rand_core::RngCore, len: (usize, usize)) -> (Vec<u8>, Vec<u8>) {
    let str_len = len.0;
    let pat_len = len.1;
    let mut vstr: Vec<u8> = Vec::with_capacity(str_len);
    let mut vpat: Vec<u8> = Vec::with_capacity(pat_len);
    for _ in 0..str_len {
        vstr.push((eng.next_u32() % 4) as u8);
    }
    for _ in 0..pat_len {
        vpat.push((eng.next_u32() % 4) as u8);
    }
    (vstr, vpat)
}

fn gen_rand_256(eng: &mut dyn rand_core::RngCore, len: (usize, usize)) -> (Vec<u8>, Vec<u8>) {
    let str_len = len.0;
    let pat_len = len.1;
    let mut vstr: Vec<u8> = Vec::with_capacity(str_len);
    let mut vpat: Vec<u8> = Vec::with_capacity(pat_len);
    for _ in 0..str_len {
        vstr.push(eng.next_u32() as u8);
    }
    for _ in 0..pat_len {
        vpat.push(eng.next_u32() as u8);
    }
    (vstr, vpat)
}
fn doit() {
    {
        // warm up
        let mut f: f64 = 0f64;
        for i in 0..1_000_000 {
            f += f * f;
            f += i as f64 * f;
        }
    }
    let mut rand01_1e3_time_kmp: Vec<f64> = Vec::with_capacity(100);
    let mut rand01_1e3_time_bm: Vec<f64> = Vec::with_capacity(100);
    let mut rand01_1e6_time_kmp: Vec<f64> = Vec::with_capacity(100);
    let mut rand01_1e6_time_bm: Vec<f64> = Vec::with_capacity(100);
    let mut rand4_1e3_time_kmp: Vec<f64> = Vec::with_capacity(100);
    let mut rand4_1e3_time_bm: Vec<f64> = Vec::with_capacity(100);
    let mut rand4_1e6_time_kmp: Vec<f64> = Vec::with_capacity(100);
    let mut rand4_1e6_time_bm: Vec<f64> = Vec::with_capacity(100);
    let mut rand256_1e3_time_kmp: Vec<f64> = Vec::with_capacity(100);
    let mut rand256_1e3_time_bm: Vec<f64> = Vec::with_capacity(100);
    let mut rand256_1e6_time_kmp: Vec<f64> = Vec::with_capacity(100);
    let mut rand256_1e6_time_bm: Vec<f64> = Vec::with_capacity(100);
    // 01string
    println!("Running rand01_1e3");
    for i in 1..100 {
        let (time_kmp, time_bm) = calc_time(10, (1_000, i), gen_rand_01);
        // print!("rand 01 string:\nkmp:{}ns\nbm:{}ns\n", time_kmp, time_bm);
        rand01_1e3_time_kmp.push(time_kmp);
        rand01_1e3_time_bm.push(time_bm);
    }
    println!("Running rand01_1e6");
    for i in 1..100 {
        let (time_kmp, time_bm) = calc_time(10, (1_000_000, i), gen_rand_01);
        // print!("rand 01 string:\nkmp:{}ns\nbm:{}ns\n", time_kmp, time_bm);
        rand01_1e6_time_kmp.push(time_kmp);
        rand01_1e6_time_bm.push(time_bm);
    }
    // ATCG
    println!("Running rand4_1e3");
    for i in 1..100 {
        let (time_kmp, time_bm) = calc_time(10, (1_000, i), gen_rand_4);
        // print!("rand 4 string:\nkmp:{}ns\nbm:{}ns\n", time_kmp, time_bm);
        rand4_1e3_time_kmp.push(time_kmp);
        rand4_1e3_time_bm.push(time_bm);
    }
    println!("Running rand4_1e6");
    for i in 1..100 {
        let (time_kmp, time_bm) = calc_time(10, (1_000_000, i), gen_rand_4);
        // print!("rand 4 string:\nkmp:{}ns\nbm:{}ns\n", time_kmp, time_bm);
        rand4_1e6_time_kmp.push(time_kmp);
        rand4_1e6_time_bm.push(time_bm);
    }
    // Full u8 char set
    println!("Running rand256_1e3");
    for i in 1..100 {
        let (time_kmp, time_bm) = calc_time(10, (1_000, i), gen_rand_256);
        // print!("rand 256 string:\nkmp:{}ns\nbm:{}ns\n", time_kmp, time_bm);
        rand256_1e3_time_kmp.push(time_kmp);
        rand256_1e3_time_bm.push(time_bm);
    }
    println!("Running rand256_1e6");
    for i in 1..100 {
        let (time_kmp, time_bm) = calc_time(10, (1_000_000, i), gen_rand_256);
        // print!("rand 256 string:\nkmp:{}ns\nbm:{}ns\n", time_kmp, time_bm);
        rand256_1e6_time_kmp.push(time_kmp);
        rand256_1e6_time_bm.push(time_bm);
    }
    println!("rand01:\n1e3:\nkmp:{:?}\nbm:{:?}\n1e6:\nkmp:{:?}\nbm:{:?}\nrand4:\n1e3:\nkmp:{:?}\nbm:{:?}\n1e6:\nkmp:{:?}\nbm:{:?}\nrand256:\n1e3:\nkmp:{:?}\nbm:{:?}\n1e6:\nkmp:{:?}\nbm:{:?}\n",
    rand01_1e3_time_kmp,
rand01_1e3_time_bm,
rand01_1e6_time_kmp,
rand01_1e6_time_bm,
rand4_1e3_time_kmp,
rand4_1e3_time_bm,
rand4_1e6_time_kmp,
rand4_1e6_time_bm,
rand256_1e3_time_kmp,
rand256_1e3_time_bm,
rand256_1e6_time_kmp,
rand256_1e6_time_bm);
}
fn main() {
    let mut bf = bloom_filter::BytesBloomFilter::new();
    for i in 1..=256 {
        let mut rd = mt19937::MT19937::new_with_slice_seed(&[Utc::now().nanosecond()]);
        let n = rd.next_u32() & 0xff;
        println!("{}th rand num:{} {}", i, n, bf.contains(&(n as u8)));
        bf.insert(&(n as u8))
    }
}
