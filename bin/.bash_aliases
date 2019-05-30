
alias rm='\rm -i'
alias cp='\cp -i'
alias mv='\mv -i'

# local additions
alias cds='cd /home/mc/Data/sf/src/stockfish-xoto10/src'
alias cdf='cd /home/mc/Data/sf/src/fishtest/fishtest/fishtest'
alias cdw='cd /home/mc/Data/sf/src/fishtest/worker'
alias cde='cd /home/mc/Data/sf/src/Ethereal/src'
alias cdl='cd /home/mc/Data/sf/src/leela-chess/src'
alias lh='ls -alFt --color=auto | head'
alias r="fc -s"

alias vi='nice vi'
alias tail='nice tail'

alias gdiff="nice git diff"
alias gdiffm="nice git diff master"
alias gdiffh="nice git diff HEAD^"
alias glog="nice git log --decorate"
alias glogh="nice git log | head"
alias gbr="nice git branch | cat"
alias gbrd="nice git for-each-ref --sort=-committerdate refs/heads/ --format='%(refname:short) %(committerdate)' | sed 's/refs\/heads\///g'"
alias gbrd2='nice git for-each-ref --sort=-committerdate refs/heads/ --format='\''%(refname:short) %(committerdate)'\'' | sed '\''s/refs\/heads\///g'\'' | awk '\''{ printf "%-16s ", $1; $1=""; print $0; }'\'' | head'
alias gbrl="nice git branch | less"
alias gbr2="nice git branch | grep -C 8 '^\*'"
alias gbr3='nice git branch --sort=-committerdate | head'
alias gch="nice git checkout"
alias gapp="nice git apply"
alias greb="nice git rebase master"
alias gtune='nice git merge official/tune -m "merge official/tune"'

alias opera='opera --password-store=basic'
alias clock='xclock -update 2 -geometry 200x200-0-0 &'

export GIT_EDITOR=vi

export PATH=$PATH:/home/mc/Data/sf/src/stockfish-xoto10/bin

# useful for virtualbox:
#
## # https://askubuntu.com/questions/834003/how-to-get-lightdm-automatically-started-at-startup-after-disabling-it-in-ubunt/834009
## 
## alias startldm='sudo systemctl start lightdm'
## 
## alias repair='sudo mongod --repair --dbpath /var/lib/mongodb/'
## 
## 
## # https://askubuntu.com/questions/834003/how-to-get-lightdm-automatically-started-at-startup-after-disabling-it-in-ubunt/834009
## # sudo systemctl start lightdm
## # sudo ln -s /lib/systemd/system/lightdm.service /etc/systemd/system/display-manager.service
## alias lh='ls -ltbF | head'
## 
## alias cdf='cd $HOME/fishtest/fishtest/fishtest'
## alias cdw='cd $HOME/fishtest/worker'
## alias rest='systemctl restart fishtest.service'
## 
## alias repair='sudo mongod --repair --dbpath /var/lib/mongodb/'
## # sudo service mongodb status
## # sudo chgrp nogroup /var/lib/mongodb/*
## # sudo chown mongodb /var/lib/mongodb/*
## # sudo rm /var/lib/mongodb/mongod.lock
## # sudo service mongodb restart
## # less /var/log/mongodb/mongodb.log 
## # 
## # smallfiles = true   [in config file]
## 

# for fishtest:
# https://docs.pylonsproject.org/projects/pyramid/en/1.5-branch/whatsnew-1.5.html  (mako)

