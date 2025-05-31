#!/bin/bash
function micros {
	s micro $@
}

function xctrl {
	if [[ "$1" = "restart" ]]; then
	    sudo /opt/lampp/lampp reloadapache && sudo /opt/lampp/lampp reloadmysql & echo "Restarted Apache and MySQL."
	elif [[ "$1" = "shutdown" ]]; then
	    sudo /opt/lampp/lampp stopapache && sudo /opt/lampp/lampp stopmysql & echo "Stopped Apache and MySQL."
	elif [[ "$1" = "start" ]]; then
	    sudo /opt/lampp/lampp startapache && sudo /opt/lampp/lampp startmysql & echo "Started Apache and MySQL."
	else
	  echo "xctrl <restart|shutdown|start>\n"
	  echo -e "  restart - restarts Apache and MySQL\n  shutdown - shuts down Apache and MySQL\n  start - starts Apache and MySQL\n"
	fi
}

function s {
	if [[ $(type $1) = function ]]
	then
		shift && command sudo bash -c "$(declare -f $1);$1 $*"
	elif [[ $(type $1) = alias ]]
	then
		alias sudo='\sudo '
		eval "sudo $@"
	else
		command sudo "$@"
	fi
}
function pips {
	s /home/asher/bin/python3 -m pip $@
}
function pys {
	s /home/asher/bin/python3 $@
}
function sqls {
	s /opt/lampp/bin/mysql -u root
}
alias echoe="echo -e"
alias phperr="micro /opt/lampp/logs/php_error_log"
alias ls="ls --color"
alias ll="ls -l"
alias lal="ls -Al"
alias la="ls -A"
alias ipc="ip -c link"
alias ipca="ip -c a"
alias cl="clear"
alias mntdeb="sudo mount /dev/nvme0n1p2 /mnt/debian && ln -s /mnt/debian $HOME/debian"
alias umntdeb="sudo umount /mnt/debian && unlink $HOME/debian"
alias ecmd="micro ~/.dotfiles/zsh/cmds.zsh"
function szsh {
	local cwd="$(pwd)"
	cd ~ && source .zshrc
	cd $cwd
}
function lamppwork {
	cd /opt/lampp/htdocs/$1
}

function set-prompt {
	zstyle ':vcs_info:*' enable git
	zstyle ':vcs_info:git*' formats '%F{226}%r/%S%f (%F{red}%b%f)%u%c'
	zstyle ':vcs_info:git*' unstagedstr '*'
	zstyle ':vcs_info:git*' stagedstr '+'
	zstyle ':vcs_info:git*' check-for-changes true

	autoload -Uz add-zsh-hook vcs_info
	add-zsh-hook precmd vcs_info

	local p_git_info='${vcs_info_msg_0_}'
	local p_time="%F{green}%T%f"
	local p_path="%F{127}%B%~%b%f"
	local p_header="%F{63}%n%f%F{105}@%f%F{69}%M%f"
	local p_prompt="%F{magenta}%(!.#.$)%f"

	local top="$p_header $p_path"
	local bottom_left="$p_time $p_prompt "
	local bottom_right="$p_git_info"

	PROMPT="$top"$'\n'"$bottom_left"
	RPROMPT="$bottom_right"
}
