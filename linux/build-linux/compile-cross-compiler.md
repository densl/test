构造工具链
# Include Three Part
# Binutils:  Binary utilities
# GCC:  compiler
# Glibc: c lib and dynamic loader/linker, newlib, etc ...

构建工具链步骤：
1. 构建交叉Binutils, 包括汇编器as, 链接器ld等。
2. 构建临时的交叉编译器（仅支持freestanding)。
3. 安装目标系统的内核头文件。
4. 构建目标系统的C库。
5. 构建完整的交叉编译器（支持hosted和freestanding)。

具体步骤
1.新建普通用户
	groupadd vita
	useradd -m -s /bin/bash -g vita vita
	vita ALL=(ALL) NOPASSWD: ALL  ## in /etc/sudoers.d add file vita

2.建立工作目录
	mkdir source build cross-tool cross-gcc-tmp sysroot
	chown -R vita.vita /vita

3.定义环境变量 in  /home/vita/.bashrc
	unset LANG
	export HOST=i686-pc-linux-gnu
	export BUILD=$HOST
	export TARGET=i686-none-linux-gnu
	export CROSS_TOOL=/vita/cross-tool
	export CROSS_GCC_TMP=/vita/cross-gcc-tmp
	export SYSROOT=/vita/sysroot
	PATH=$CROSS_TOOL/bin:$CROSS_GCC_TMP/bin:$PATH

4.构建二进制工具
	mkdir binutils-build
	../binutils/configure \
		--prefix=$CROSS_TOOL --target=$TARGET \
		--with-sysroot=$SYSROOT

	make
	make install

5.编译freestanding的交叉编译器
	mkdir gcc-build
	../gcc/configure \
	  --prefix=$CROSS_GCC_TMP --target=$TARGET \
	  --with-sysroot=$SYSROOT \
	  --with-newlib --enable-languages=c \
	  --with-mpfr-include=/vita/build/gcc/mpfr/src \
	  --with-mpfr-lib=/vita/build/gcc-build/mpfr/src/.libs \
	  --disable-shared --disable-threads \
	  --disable-decimal-float --disable-libquadmath \
	  --disable-libmudflap --disable-libgomp \
	  --disable-nls --disable-libssp

	make
	make install

	ln -s libgcc.a lib/gcc/i686-none-linux-gnu/4.7.2/libgcc_eh.a

6.安装内核头文件
	make mrproper
	make ARCH=i386 headers_check
	make ARCH=i386 INSTALL_HDR_PATH=$SYSROOT/usr/  headers_install

7.编译目标系统的C库
	mkdir glibc-build
	../glibc/configure \
	  --prefix=/usr --host=$TARGET \
	  --enable-kernel=3.7.4 --enable-add-ons \
	  --with-headers=$SYSROOT/usr/include \
	  libc_cv_forced_unwind=yes libc_cv_c_cleanup=yes \
	  libc_cv_ctors_header=yes

	make
	make install_root=$SYSROOT install

	##pathch
	patch -p1 < ../../source/glibc-2.15-cpuid.patch
	patch -p1 < ../../source/glibc-2.15-s_frexp.patch

8.构建完整的交叉编译器
	rm gcc-build/* -rf
	../gcc/configure \
	  --prefix=$CROSS_TOOL --target=$TARGET \
	  --with-sysroot=$SYSROOT \
	  --with-mpfr-include=/vita/build/gcc/mpfr/src \
	  --with-mpfr-lib=/vita/build/gcc-build/mpfr/src/.libs \
	  --enable-languages=c,c++ --enable-threads=posix

	make
	make install

9.定义工具链相关的环境变量
	export CC="$TARGET-gcc"
	export CXX="$TARGET-g++"
	export AR="$TARGET-ar"
	export AS="$TARGET-as"
	export RANLIB="$TARGET-ranlib"
	export LD="$TARGET-ld"
	export STRIP="$TARGET-strip"

	export DESTDIR=$SYSROOT

10.封装程序依赖的共享库 pkg-config
	unset PKG_CONFIG_PATH
	export PKG_CONFIG_LIBDIR=$SYSROOT/usr/lib/pkgconfig: \
		$SYSROOT/sur/share/pkgconfig

	pkg-config 放在 $SYSROOT/bin:

	#!/bin/bash
	HOST_PKG_CFG=/usr/bin/pkg-config
	
	if [ ! $SYSROOT ]; then
		echo "please make sure in cross-compile environment！"
		exit 1
	fi

	$HOST_PKG_CFG --exists $*
	if [ $? -ne 0]; then
		exit 1
	fi

	if $HOST_PKG_CFG $* | sed -e "s/-I/-I\/vita\/sysroot/g;\
		s/-L/-L\/vita\/sysroot/g"
	then
		exit 0
	else
		exit 1
	fi


将库的链接如la文件删除：
	find $SYSROOT -name "*.la" -exec rm -f '{}' \;




others for c:
	.preinit_array         
	.init_array
  .c file:
	void myinit(int argc, char **argv, char **envp)
	{
		printf("%s\n", __FUNCTION__);
	}
	__attribute__((section(".init_array"))) typeof(myinit) *_myinit=myinit;
	
  compile:
	gcc hello.c
	./a.out



其它相关资料：
	blog.163.com/wflk_wangwei/blog/static/2788833720144252959686

