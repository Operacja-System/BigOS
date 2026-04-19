Arch Linux
==========

To install all necessary packages run:

    .. code-block:: bash

        sudo pacman -Sy --needed \
            riscv64-linux-gnu-gcc \
            dosfstools  \
            e2fsprogs \
            mtools \
            'qemu-system-riscv>=10.1.0' \
            'cmake>=3.29.0' \
            'clang>=21.0.0' \

To locally build documentation, You will also need:

    .. code-block:: bash

        sudo pacman -Sy --needed \
            'doxygen>=1.16.0' \
            'python-sphinx>=8.2.0' \
            'python-breathe>=5.0.0' \
            'python-pydata-sphinx-theme>=0.16.0' \

To run `./scripts/fix-include-what-you-use.sh` You will also need a package called "include-what-you-use".
It is not available on official repos, so You will need to download it from `AUR <https://wiki.archlinux.org/title/Arch_User_Repository>`_
using a helper, such as, `yay <https://github.com/jguer/yay>`_.
