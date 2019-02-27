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

## 2 解决git膨胀过大的问题

### 2.1 git膨胀的原因

- git是Source Configuration Management(SCM)工具, 会保存所有的二进制差异。
- git只适合存放source(源码), 不适合存放artifact(制成品)。
- 构建过程中需要依赖的lib/image/video等，建议存放在artifact repository(仓库)。
​
### 2.2 江湖救急方案: 删除大文件提交记录。

查看存储库中的大文件：

``` shell
rockit $ git rev-list --objects --all | grep -E `git verify-pack -v .git/objects/pack/*.idx | sort -k 3 -n | tail -10 | awk '{print$1}' | sed ':a;N;$!ba;s/\n/|/g'`
```

改写历史，去除大文件

``` shell
rockit $ git filter-branch --tree-filter 'rm -f path/to/large/files' --tag-name-filter cat -- --all
rockit $ git push origin --tags --force
rockit $ git push origin --all --force
rockit $ git pull rebase
```

并告知所有组员，push 代码前需要 pull rebase，而不是 merge，否则会从该组员的本地仓库再次引入到远程库中。

## 3 参考资料

- [Git+Gerrit如何永久删除历史文件](https://www.jianshu.com/p/085552205f19)


