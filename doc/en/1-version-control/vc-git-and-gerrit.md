# Best Practice for Git and Gerrit Version

## 1 Create and initialize the Gerrit project

- Create a Rockit project in the browser
- Clone the Rockit project to the local and initialize the empty Rockit project
- Keep 2 remote branches, Gerrit does code review, syncs to github branch after review is done

```
$ git clone ssh://cmc@10.10.10.100:29418/Rockit Rockit.git
$ cd Rockit.git
$ git remote add github https://github.com/media-tm/Rockit
$ git remote -v
$ git fetch github
$ git checkout github/master -b github_master
$ git push origin github_master:master  # push github/master to origin/master branch
$ git checkout origin/master -b origin_master
```
