build kernel steps:
    1) 配置内核 => 产生 .config文件；
    2) 保存内核版本到include/linux/version.h文件中；
    3) 符号链接include/asm to include/asm-$(ARCH)；
    4) 更新所有目标对象的其它前提文件，附加前提文件定义在arch/$(ARCH)/Makefile文件中；
    5) 递归进入init- core- drivers- net- libs-中的所有子目录和编译所有的目标对象，上面变量值都引用到arch/$(ARCH)/Makefile文件中；
    6) 链接所有的object文件生成vmlinux文件，vmlinux文件放在代码树根目录下。最开始链接的几个object文件列举在arch/$(ARCH)/Makefile文件的head-y变量中；
    7) 最后体系Makefile文件定义编译后期处理规则和建立最终的引导映像bootimage。包括创建引导记录，准备initrd映像和相关处理。




makefile包括五部分：

    Makefile　顶层Makefile文件
    .config　内核配置文件
    arch/$(ARCH)/Makefile　机器体系结构Makefile文件
    scripts/Makefiles　所有内核Makefiles共用规则，与Makefile文件一起编译出内核程序。
    kbuild Makefiles　其它Makefile文件

在make menuconfig进行内核配置操作时，所显示的Menu list是由各层Kconfig组成的。最底层Kconfig存放在 ~/arch/i386/Kconfig. 以此为头，它会一层层使用source来把需要加入的各个目录中Keconfig添加近来。例如：source "drivers/Kconfig",则将~/drivers/Kconfig添加进Menu list中。通过内核配置操作产生.config文件.

顶层Makefile文件读取该文件的配置。顶层Makefile文件负责产生两个主要的程序：vmlinux (内核image)和模块。顶层Makefile文件根据内核配置，通过递归编译内核代码树子目录建立这两个文件。顶层Makefile文件文本一个名为 arch/$(ARCH)/Makefile的机器体系结构Makefile文件。机器体系结构Makefile文件为顶层Makefile文件提供与机 器相关的信息。每一个子目录有一个Makefile文件，子目录Makefile文件根据上级目录Makefile文件命令启动编译。这些 Makefile使用.config文件配置数据构建各种文件列表，并使用这些文件列表编译内核或模块的目标文件。



    用户 用户使用"make menuconfig"或"make"命令编译内核。他们通常不读或编辑内核Makefile文件或其他源文件。
    普通开发者 普通开发者维护设备驱动程序、文件系统和网络协议代码，他们维护相关子系统的Makefile文件，因此他们需要内核Makefile文件整体性的一般知识和关于kbuild公共接口的详细知识。
    体系开发者 体系开发者关注一个整体的体系架构，比如sparc或者ia64。体系开发者既需要掌握关于体系的Makefile文件，也要熟悉内核Makefile文件。
    内核开发者 内核开发者关注内核编译系统本身。他们需要清楚内核Makefile文件的所有方面。

