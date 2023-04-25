# STM32G431-Motor
hust计控课设，驱动三相无刷电机并实现PI调速
此文件编码格式 UTF-8

# 开发环境
cubemx6.5.0
keil5:在keil5中记得将user文件夹的文件加入工作区，并且加入include读取路径

# 注意事项
在.gitignore文件中现在不忽略追踪了
记得一定用
git branch -m master main
把git默认的master分支重命名为main之后再进行操作

# git步骤(只操作main)
git init
git branch -m master main
git remote add github的地址
git pull --rebase origin main
git add .
git commit -am "注释"
git push origin main

# 新建分支
git branch 分支
git checkout 分支名切换到改分支
git merge 分支名    融合
git branch -D 分支名    删除分支
分支提交git push origin 分支名
在github里面进行pull requests将分支与main合并

# 以上可能还是有错误！
