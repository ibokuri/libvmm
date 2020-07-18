# **Libvmm**

Libvmm provides common virtualization components for Virtual Machine Monitor
(VMM) and hypervisor development.

Despite VMMs and hypervisors often sharing certain primitives and components
(e.g., VM lifecycle/resource management via KVM, I/O virtualization via
virtio), these shared pieces are typically reimplemented for each project,
resulting in duplicated efforts and boring drudge work. The goal of Libvmm is
to prevent this redundancy.

## Installation

#### Manual
To build and install from source:

```
git clone https://git.sr.ht/~satchmo/libvmm && cd libvmm
meson build
ninja -C build
sudo ninja -C build install
```

<!--Make sure that wherever meson installs the library files to (typically-->
<!--/usr/local/) is in `ldconfig`'s path. Otherwise, you'll get a `libvmm.so: No-->
<!--such file or directory` error when running any programs using Libvmm. To fix-->
<!--this, simply add the library path to `ldconfig`'s configuration file:-->

<!--```-->
<!--$ echo "/usr/local/lib" | sudo tee /etc/ld.so.conf.d/local.conf-->
<!--$ sudo ldconfig-->
<!--```-->

#### Packages

Libvmm is available in many Linux distributions (as `libvmm`), including:

* Arch Linux
* CentOS
* Debian
* Fedora
* RHEL
* Ubuntu

## Contributing
See [CONTRIBUTING.md](CONTRIBUTING.md).

## License
This project is released under the [MIT](LICENSE) license.
