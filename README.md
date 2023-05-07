# STM32G431-Motor
hust计控课设，驱动三相无刷电机并实现PI调速

此文件编码格式 UTF-8

# 开发环境
MC_SDK5.4.0+cubemx6.5.0+keil5

# 注意事项
在.gitignore文件中现在不忽略追踪了

记得一定用git branch -m master main把git默认的master分支重命名为main之后再进行操作

Keil5代码优化等级选择O1

TIM2_IRQHandler()在文件"stm32g4xx_it.c"

# 实验进展
跟着"MC_SDK5.4"此ppt做完了试验1,2,3

# 看这里理清思路
首先不再需要自己去编写pi算法了

如果有时间精力的话可以尝试增加例如积分分离等不一样的pi控制方法（优先级很低）

主要任务，用pilot观察检测的速度和给定速度

更改kp，ki，这个看起来比改变pi算法靠谱

除了阶跃响应，还可以尝试斜坡响应，那个什么s型曲线是什么我不清楚但是可以先放在心上

进阶，adc采样滤波，在adc中断中把采集数据存进数组（优先级低）

然后主要就是在呈现的时候体现出自己的工作量

# 重点
要做好展示环节体现工作量：

1分工，包括git的提交记录

2.做matlab仿真，应该就可以按照欧拉电子的教程的那个matlab仿真去做

虽然这没有卵用但是可以让我们看起来做了一些事情

3.以上通过ppt展示（应该）

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
