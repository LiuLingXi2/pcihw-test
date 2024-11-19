# pcihw-test

## nvme architect

```shell
/ # lspci -v
00:00.0 Class 0600: Device 1b36:0008
00:01.0 Class 0200: Device 1af4:1000
00:02.0 Class 0c03: Device 1b36:000d (rev 01)
00:03.0 Class 0604: Device 1b36:0001
01:04.0 Class 0108: Device 1b36:0010 (rev 02)
01:05.0 Class 0108: Device 1b36:0010 (rev 02)

01:04.0 Class 0108: Device 1b36:0010 (rev 02) (prog-if 02)
        Subsystem: Device 1af4:1100
        Flags: bus master, fast devsel, latency 0, IRQ 49, NUMA node 0
        Memory at 10000000 (64-bit, non-prefetchable) [size=16K]
        Capabilities: [40] MSI-X: Enable+ Count=65 Masked-
        Capabilities: [80] Express Endpoint, MSI 00
        Capabilities: [60] Power Management version 3
        Kernel driver in use: nvme

01:05.0 Class 0108: Device 1b36:0010 (rev 02) (prog-if 02)
        Subsystem: Device 1af4:1100
        Flags: bus master, fast devsel, latency 0, IRQ 55, NUMA node 0
        Memory at 10004000 (64-bit, non-prefetchable) [size=16K]
        Capabilities: [40] MSI-X: Enable+ Count=65 Masked-
        Capabilities: [80] Express Endpoint, MSI 00
        Capabilities: [60] Power Management version 3
        Kernel driver in use: nvme
```

It is clear from the above that the nvme enable msi-x interrupt, and we can look the mis-x number by its mechanism from gic by default(may be spi(>8192))

 
## busybox




## pciutils

The lspci tool belong to busybox is so simple, so we need to have the protozoa lspci by compile pciutils

github: https://github.com/pciutils/pciutils

compile step:
- modify Makefile: 
        ```shell
        # Host OS and release (override if you are cross-compiling)
        HOST=arm-linux
        RELEASE=
        CROSS_COMPILE=aarch64-linux-gnu-

        # Support for compressed pci.ids (yes/no, default: detect)
        ZLIB=no

        # Support for resolving ID's by DNS (yes/no, default: detect)
        DNS=no
        ```
- make -j$(nproc)
- if the default file type is dynamic shared, but we need static, so we may add the compila flags: CFLAGS="-static" LDFLAGS="-static".(i can not find the LDFLAGS, so simply add the compile flags mandatory)