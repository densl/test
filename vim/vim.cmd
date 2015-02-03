
## vim commands


## fx  :移动到一个指定的字符 
        To err is human.  To really foul up you need a computer.
        ---------->--------------->
            fh           fy 
## 可以通过 ";" 命令重复，"," 命令则用于反向重复 

## %  :括号匹配

## nG  :移动到指定的行
## n%  :移动到指定百分比

## H ，  M，  L ：移动到视野之内的一行(Home, Middle, Last)

## ctrl+g  :确定当前位置，文件信息

## .  :重复操作

## !cmd or  %!cmd :执行shell命令

## 替换第n行开始到最后一行中每一行所有 old 为 new
	:n, $s/old/new/g

## 替换当前行所有 old 为 new
	:s/old/new/g

## 安装Ctags
1.进入源代码目录执行 ./configure
2.执行 make && make install
3.在 ~/.vimrc 增加一行 map <C-F12> :!ctags -R --c++-kinds=+p --fields=+ias --extra=+q .<CR>
4. ctrl-] 跳转到光标所在符号的定义
5. ctrl-t 回到上次跳转前的位置
6. 增加一行到vimrc:   set tags=tags;
## 安装Taglist
1.解压到~./vim目录，无则新建一个
2.在doc目录执行命令 "helptags ."
3.在vimrc增加	
	let Tlist_Show_One_File=1  
	let Tlist_Exit_OnlyWindow=1
	let Tlist_Use_Right_Window=1
	let Tlist_Ctags_Cmd='/usr/local/bin/ctags'
