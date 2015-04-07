set arch arm
target remote :1234
set print pretty

define load_el3
file arm/el3/el3.elf
add-symbol-file arm/el3/el3.elf &_EL3_TEXT_BASE
end

define load_el1s
file arm/el1_s/el1_sec.elf
add-symbol-file arm/el1_s/el1_sec.elf &_EL1_S_TEXT_BASE
end

define load_el1ns
file arm/el1_ns/el1_nsec.elf
add-symbol-file arm/el1_ns/el1_nsec.elf &_EL1_NS_TEXT_BASE
end

define load_el0ns
add-symbol-file aarch64/el0_ns/el0_nsec.elf 0x80000
end

define load_el0s
add-symbol-file aarch64/el0_s/el0_sec.elf 0xc0000
end
