<!--<p align="center"><img src="" width="350px"></p>-->

<h1 align="center">Libvmm</h1>

<p align="center">An umbrella library for VMM and hypervisor development.</p>

<!--<p align="center">-->
  <!--<a href="https://lgtm.com/projects/g/libvmm/libvmm/context:cpp">-->
  <!--<img src="https://img.shields.io/lgtm/grade/cpp/g/libvmm/libvmm.svg?logo=lgtm&logoWidth=18" alt="Language grade: C/C++" />-->
  <!--</a>-->
<!--</p>-->

<!--VMMs and hypervisors often share a large portion of their core functionality-->
<!--(e.g., VM lifecycle and resource management via KVM, I/O virtualization via-->
<!--virtio). However, these shared components are typically reimplemented for each-->
<!--project, resulting in duplicated efforts and unecessary drudge work. Libvmm-->
<!--aims to help developers avoid this sort of redundancy.-->

## Installation

### Manual

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
<!--echo "/usr/local/lib" | sudo tee /etc/ld.so.conf.d/local.conf-->
<!--sudo ldconfig-->
<!--```-->

<!--### Packages-->

<!--Libvmm is available on many Linux distributions (as `libvmm`), including:-->

<!--* Arch Linux-->
<!--* CentOS-->
<!--* Debian-->
<!--* Fedora-->
<!--* RHEL-->
<!--* Ubuntu-->

## Contributing
See [CONTRIBUTING.md](CONTRIBUTING.md).

## License
This project is released under the [MIT](LICENSE) license.
