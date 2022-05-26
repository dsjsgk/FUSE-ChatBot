# ChatBot(Based On FUSE)

实现了一个基于FUSE的聊天系统，如果希望实现A和B的聊天，首先需要在主目录（或任意其他目录下建立两个文件夹）

`mkdir A`

`mkdir B`

如果希望A向B发送消息

`echo "hi B" > A/B`

会在A中建立一个新文件B，同时在B中建立一个新文件A，新文件记录着聊天信息。



