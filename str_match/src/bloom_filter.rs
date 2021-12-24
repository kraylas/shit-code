pub struct BytesBloomFilter {
    mask: u64,
}

impl BytesBloomFilter {
    pub fn new() -> Self {
        BytesBloomFilter { mask: 0 }
    }

    pub fn insert(&mut self, byte: &u8) {
        (self.mask) |= 1u64 << (byte & 63);
    }

    pub fn contains(&self, byte: &u8) -> bool {
        (self.mask & (1u64 << (byte & 63))) != 0
    }
}
