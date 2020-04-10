
#$ARM-objcopy -O binary t.elf t.bin
riscv-qemu-4.1.0-2019.08.0-x86_64-linux-ubuntu14-sifive/riscv-qemu-4.1.0-2019.08.0-x86_64-linux-ubuntu14/bin/qemu-system-riscv64 -machine virt -bios none -kernel httpd.app -m 128M -smp 1 -nographic -netdev user,id=net0,hostfwd=tcp::26999-:80 -object filter-dump,id=net0,netdev=net0,file=packets.pcap -device e1000,netdev=net0,bus=pcie.0

#qemu-system-riscv64 -machine virt -bios none -kernel t.elf -nographic
