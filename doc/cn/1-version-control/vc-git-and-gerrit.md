# 版本控制: GIT&Gerrit最佳实践

## 1 创建和初始化Gerrit工程

- 再浏览器中创建Rockit工程
- 克隆Rockit工程到本地，并初始化空的Rockit工程
- 保持2个远程分支，Gerrit做代码审核，审核完成之后同步到github分支

``` shell
rockit $ git clone ssh://cmc@10.10.10.100:29418/Rockit Rockit.git
rockit $ cd Rockit.git
rockit $ git remote add github https://github.com/media-tm/Rockit
rockit $ git remote -v
rockit $ git fetch github
rockit $ git checkout github/master -b github_master
rockit $ git push origin github_master:master  # github/master分支push到origin/master分支
rockit $ git checkout origin/master -b origin_master
```
