

  .show remote info
  git remote show origin
 
  .add files
  git add --all
  git add .                   #add modified files

  .show status
  git status

  .set user git config --global user.name yourUserName
  git config --global user.email yourUserEmail

  .commit alter
  git commit -m YourMessage   #will commit all changes
  git commit -m "message" -e  #edit what files
  git commit --status         #show commit status

  .push to remote
  git push -u origin
  git push -u origin master   #push branch only master

  .switch branch
   git checkout <branch>

  .pull remote changes
  git pull -v origin master
  git pull -v --all            #pull all branches

  .config commit
  git config --global push.default simple 
  

vim commands:
^   :go to line head
$   :go to line end
gg  :go to first line 
G   :go to last  line
u   :undo


ctrl+v  : enter visual mode 
ctrl+r  : redo