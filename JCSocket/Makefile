#编译器名字
CompilerJC = g++

#指定根目录 o文件生成路径
RootPathJC = .
OPathJC = obj

#目标名字
ObjectJC = ./Test


#创建目标目录
$(shell mkdir -p $(OPathJC))

#获取指定根路径下的所有子路径集合 搜索深度
SearchDeepJC = 5
SonPathJC = $(shell find $(RootPathJC) -maxdepth $(SearchDeepJC) -type d)

#设置编译参数 链接库 VPATH
CFLAGS = -std=c++11 -g
LDFLAGS =
SPACE =  
VPATH = $(subst $(SPACE),, $(strip $(foreach pathJC, $(SonPathJC), $(pathJC)))) $(OPathJC)

#获取当前目录下的所有带路径的cpp文件集合
PathCppFileSetJC = $(foreach pathJC, $(SonPathJC), $(foreach fileJC, $(pathJC), $(wildcard $(pathJC)/*.cpp)))
CppFileSetJC = $(notdir $(PathCppFileSetJC))

#映射当前目录下的所有cpp文件o文件集合
OFileSetJX = $(patsubst %.cpp, $(OPathJC)/%.o, $(CppFileSetJC))

#生成目标文件
$(ObjectJC) : $(OFileSetJX)
	$(CompilerJC) $(CFLAGS) -o $@ $(OFileSetJX) $(LDFLAGS)
	#******************************************************************************#
	#                        Test Bulid successful !                             #
	#******************************************************************************#
	
#生成临时目标文件
$(OPathJC)/%.o : %.cpp
	$(CompilerJC) $(CFLAGS) $< -c -o $@
	
    
#清除目标 临时目标
clean:
	rm -rf $(OPathJC)/*.o
	rm -rf $(ObjectJC)