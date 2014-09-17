

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
  

#add ssh key
#Creates a new ssh key, using the provided email as a label
  ssh-keygen -t rsa -C "your_email@example.com"

#enter your passphase
#add new key to ssh-agent
  ssh-add ~/.ssh/id_rsa
#add your ssh key to github
  clip < ~/.ssh/id_rsa.pub #copy contents to your clipboard

#add SSH key in GitHub
#Test everything out
  ssh -T git@github.com

#
#push to remote
  git remote add test git@github.com:username/repo.git
  git push test master




vim commands:
^   :go to line head
$   :go to line end
gg  :go to first line 
G   :go to last  line
u   :undo


ctrl+v  : enter visual mode 
ctrl+r  : redo
