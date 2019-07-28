=========================================================================================================
                              Positional argument
=========================================================================================================
${*} all argumets are passed as 1 argument, separated with the first character of $IFS 
${@} all arguments passed quoted and space separated, like "$1" "$2" "$3"
This expansions doesnt return $0, and if no argument is passed, expands to nothing
The first form is usefull for outputing, when you want to return results separated by
same character, like IFS=',' echo $*
The seconf form is used when you want to forward the same list of argument to another
function

To set positional arguemnts from program use `set`
set one two three four #set cmdline args

${#}        #number of args
${!#}       #last arg
${#P}       #string len
${#ARR[@]}  #arr len
${#ARR[1]}  #first element len

Substring expansion
${P:OFFSET}         #expends till end
${P:OFFSET:LENGTH}  #             LENGTH
                    # can be used with negative offsets but requires '-' to be separated
${P:(-1):0}         # returns entire word
                    # negative length goes from end to beginning not like offset
Array handling
array=(This is a text)
${array[0]:2:2} # is (first word)
${array[@]:1:2} # is a

=========================================================================================================
                                     Arrays
=========================================================================================================
There are two types of arrays in bash, indexed and associative  In indexed keys are ordered integers,
while the latter are attays in which the keys are represented by strings. Associated arrays can be
initialized only using declare command

declare -a os                   # declare indexed array
os=('ubuntu' 'mac')             # create/initialize indexed array
os[3]='windows'                 # set element by index
os+=(windows)                   # add element to array end
unset os[3]                     # unset elemnt by index
echo "${os[@]}"                 # get all elements
echo "${!os[@]}"                # get all keys
echo "${#os[@]}"                # get array length

declare -A assocarr             # declare associated array
assocarr[Var1]="Var1"           # set element by key
assocarr+=([key]=val)           # add element to array
unset assocarr[key]

=========================================================================================================
                                Typed variables
=========================================================================================================
If you want to define type of variable use declare with options. Isue  `declare` , where '-' enable and
'+' dissables attribute.  Next type attributes can be used
-a  ## indexed array
-A  ## associative array
-i  ## integer

As well, attributes not related to type
-r  ## read-pnly
-x  ## export
-n  ## dereference value?
-t  ## trace usage
-u  ## convert used to name to upper case
-l  ## convert used to name to lower case

=========================================================================================================
                              Integer arithmetics
=========================================================================================================
To execute simple calculations with integeres you need to use arithmetic environment or declare variables
as integers. For instance

var one=1 two=2
three=one+two
echo $three # will return 12

But if you declare one and two with `declare -i` or with let, plus opearator will perform arithmetic
addition instead of concatination. Let is almost the same with (()). It can used to declare and initalize
variable the same time like
let var+=bar        # undefined variable == 0
Declare only sets integer attribute for variable (will arthimetic operations work within declare assignment)

Bash doesnt supports floating point numbers, use substitution with programs that know how to wotk
with them, like `bc`

To perform arithmetic expansiong, you can use `expr $var <op> $var`
Another way is arthimetic in double parantheses  $(($var <op> $var)), variable dereferencing is optional
x=$(( 1+2 )) # assignment requires $ before parantheses
((x += 1))   # simple opearation doesnt require $
Otherwise, u can initialize varialbe with let
let x=x+3
let "x = x+3"   # allowes spaces withing quotation

=========================================================================================================
                              Indirect References
=========================================================================================================
Bash gives us ability to reference one varible from another, for instance `a=123 ;b=a ;`
This can be though of as pointers

To derefernce such varialbe you have couple of ways
Declare new variable as result of derefencing another,
declare -n var=${dereference_var}   # will expand all indirection till constant?
Eval variable with one extra esacaped \$
eval echo \$${var}
If shell supports, expand with !
echo ${!var}

=========================================================================================================
                                 Traps Signals
=========================================================================================================
when a script is terminated by a signal, exist status will be the result of 128 + the signal number
trap -l     # display signals with their numbers
kill -l     # same  
trap -p     # display list of set traps

Pseudo signals
EXIT        # executed on shell exit
DEBUG       # executed before every simple command
RETURN      # executed when a shell function or a sourced code finishes executing
ERR         # executed each time a command's failure would cause the shell to exit 
            # when the ''-e'' option (''errexit'') is enabled 

=========================================================================================================
                                   Debugging
=========================================================================================================
The minus is used to activate a shell option and a plus to deactivate it.
bash -x scritpt     # will print value of each expansion / cmd
set -x / +x         # can be used within scripts
set -f              # Disable file name generation using metacharacters (globbing)
set -v              # Prints shell input lines as they are read.

To get call stack, call `caller` in loop

Tracing variable
declare -t VARIABLE=value
trap "echo VARIABLE is being used here." DEBUG
=========================================================================================================
                              Value manipulations
=========================================================================================================
In shell any variable can be displayed as ${name_of_the_variable} instead of $name_of_the_variable. This
notation was initially introduced to protect a variable name from merging with string that comes after it,
but was extended to allow string operations on variables.

Works for each element of array id index is not specified
${P^}   # conver first letter to upercase
${P,}   #                        lovercase
${P~}   #                        inverse
${P^^}  # entire word convertion
${P,,}  #
${P~~}

${!PREFIX*}     # returns all var starting with prefix
${!PREFIX@}

Remove patern from name
${P#PATTERN}    # from beggining shortest match
${P##PATTERN}   #        longest
${P%PATTERN}    # from end   shortest
${P%%PATTERN}   #        longest

Search and replace (works for arrays)
${P/PATTERN/STR}    # replace first occur
${P//PATTERN/STR}   #         all
${P/ or //PATTERN}  # remove

Anchoring? (replacement will be started after anchor)
MYSTRING=xxxxxxxxxx
${MYSTRING/#x/y}  # RESULT: yxxxxxxxxx
${MYSTRING/%x/y}  # RESULT: xxxxxxxxxy

Default value
${P:-WORD}  # return word if P is not set or null
${P-WORD}   # returb word only if unset

Arrays
contains no element or is unset (the :- and - variants mean the same here)
contains only elements that are the nullstring (the :- variant)

If not set, set with value
#not working for arrays
${P:=WORD}
${P=WORD} 

${P:+WORD}
${P+WORD} 

Null/Unset  - display error appending word
${P:?WORD}
${P?WORD} 
=========================================================================================================
                      Redirection
=========================================================================================================
When you redirect some file, bash opens a couple file descriptors, you can think of them as file ports that
are connected to some another port like user terminal. This connection can be use to read data in and
return data out. When file is opened it has three associated fds(file descriptors) by default. These are
0  ====\                    ## standart input 
1  ====== /dev/tty0         ## standart output
2  ====/                    ## standart error
By default, stdin(0) is user input, probably from some terminal like window; stdout(1) and stderr(2) is
where data is printed, as well probably your terminal. You can reassing there ports on process start,
therefore data can be read like as user typed it in or you can save output to file. 
There are even more advanced associations, like redirecting input to process or output from process.

This default behaviour is an effect of fork process wich makes child process inherit fds. In general it
works next way: user opens some terminal emulator which is responsible for starting default shell and
configuting its file descriptors to point where it wants (probably output to slave part and input to master?)
Then you execute some command, for which, shell forks subprocess whose fds are inherited from shell and
point to terminal. If user provided some other redirections, they are applied with `exec`.
Child process is free to change fds as it wants, or allowed.

Redirection of the form `echo 123 > file` is equal to `echo 123 1>file` which will assign stdout of echo
to file. There is no such alias for stderr therefore we need to explicitly redirect `echo 123 2>file` .If
u provide descriptor greater then 2, it will open new file descriptor which can be configured by user.

Duplicating => making multiple file descriptors point to one file, for instance `ls 123 2>&1 | vim`
This duplication can be read as, make stderr point to thing pointed by stdin This is not named aliasing
since we change files, and after change of stderr we can change stdout so they will point to different files
ls exist not_exist 2>&1 1>log               # will print err to stdout and !err to log file

Ordering => redirections are set up from left to right
cmd 2>&1 > file  # redirect stderr to user and stdout to file
cmd >file 2>&1   # redirect stdout to file and make stderr point to file
Following examples has the same result
$ echo hello >/tmp/example
$ echo >/tmp/example hello
$ >/tmp/example echo hello

Typycal error during redirection is file truncating. Stdout redirection '>' will truncate file before
command is executed, there it is not a good idea to use file as argument and redirct to it sametime
sed 's/foo/bar/' file > file  # file is trucated during fd redirections

To modifie file descriptors of the current shell use 'exec'
exec 2>file

Using additional fds can be used during usage of nested shells, for instance
$ while read -r line;do echo "$line"; read -p "Press any key" -n 1;done < file
Here read for pressed key will get input from file. We can fix this by involving additional fd
$ exec 3<file
$ while read -u 3 line;do echo "$line"; read -p "Press any key" -n 1;done

To close a file descriptor, point it to '-', if you try to write to closed fd, error will be returned
$ ls 1>&-

---------------------------------------------------------------------------------------------------------
                            Non standart extensions
---------------------------------------------------------------------------------------------------------
There is '>& / >>&' and '&> / &>>' which redirect both stdout and stderr. Diffence between them is next
'&>' came from bash and '>&' came from csh. Both variants shouldnt be used for portable sh scripts

---------------------------------------------------------------------------------------------------------
                       Multiline input (Here Doucuments)
---------------------------------------------------------------------------------------------------------
There is option for multiline input, which can be done in next way
command << DELIMITER
   ...
DELIMITER

This can be used to hold file in script, so you dont need to create it outsied and pipe into script or
for progrmas with interactiv input like ftp
ftp << DEL
user ...
get file path
...
DEL

If u want to pipe this command into another, pipe should be used after first delimiter
command << DELIM | command2 | ...
  ...
DELIM

---------------------------------------------------------------------------------------------------------
                          Passing variables to command
---------------------------------------------------------------------------------------------------------
There are different ways to use variable as input to command for instance
echo $foo | sed 's/a/A/'
But there is one more readable form
sed 's/a/A' <<< $foo
read a b c <<< "$params"
---------------------------------------------------------------------------------------------------------
                     Pipe (collaboration but not chaining)
---------------------------------------------------------------------------------------------------------
Pipe is a shell builtin mechanism that allows connect STDIN of one command with STDOUT of another and
run them simultaneously, therefore command like `sleep 3 | sleep 3 | sleep 3` takes only 3 seconds, or
`ps | grep ps`  will output `grep ps` as one of the results.
Note that the pipe is a unidirectional communication channel. Thus, data can only flow in one direction

Pipe connects standart output of one process to standart input of another therefore commands like
`ls | echo` woun`t work since echo takes command line arguments and not stdin. There is workaround for
such cases, like
ls | xargs echo
echo $(ls)

There are three types of pipes
1. Named, also called FIFO. They have name and they exist on filesystem untill mannually removed.
   They don`t depent on process running therefore can be reused for multiple purposes.
   Created with mknode() and removed with `rm` or unlink
2. Unnamed.  Any command of type `cmd | cmd` creates unnamed pipe, since it is no more needed after
   executiong and can be cleaned. It is created using `int pipe(int fd[2])` and closed using `close()`
3. Annonymous named

Chile processes inherit open file descriptors. This is why pipes work. To prevent an fd from being
inherited, close it.

Pipe commands are oppened in a subshell, therefore variables used inside pipes are not defined after pipe
    echo "hello" | read first
    echo "world" | read second | echo $second # nothing
    echo $second $first # nothing

Workaround for this can be additional scope
    echo "hello world" | {
        read first second
        echo $second $first # world hello
    }
If u want variable to be in a subshell, user hear-string
read first second <<< "hello world"

Shell can be configured to not overwrite file on > redirection, in this case u need manually clean file
or use ">| file" redirection.

Named pipes can be used for multiple purposes, one interesting is work around of pipe unideriction
(data can go only from left to right ??? TODO clarify)
# using netcat as a proxy
mkfifo backpipe
nc -l 12345  0<backpipe | nc www.google.com 80 1>backpipe

https://stackoverflow.com/questions/6893714/why-does-ps-o-p-list-the-grep-process-after-the-pipe
https://stackoverflow.com/questions/9834086/what-is-a-simple-explanation-for-how-pipes-work-in-bash
http://www.cs.fredonia.edu/~zubairi/s2k2/csit431/pipes.html
---------------------------------------------------------------------------------------------------------
                              Process redirection
---------------------------------------------------------------------------------------------------------
You can pipe or redirect result of one program to another, but what if you want to redirected result of
multiple commands to one. In this case use process redirection. This looks like `diff -u <(ps) <(ps -e)`
where `<(ps)` creates an anonymous named pipe, and connects stdout of the process to the write part of
the named pipe. Then bash executes the process, and it replaces the whole process substitution expression
with the filename of the anonymous named pipe. Special file  When bash sees `<(ps it opens a special file
`/dev/fd/n`, where n is a free file descriptor, then runs `ps` with its stdout connected to /dev/fd/n
and replaces `<(ps)` with /dev/fd/n so the command effectively becomes:
$ diff -u /dev/fd/n1 /dev/fd/n2



There is aswell stdin redirection, so output of your command will be redirected to another command which
is commonly used for filtering
ps -ef | tee >(awk '$1=="tom"' >toms-procs.txt) \
               >(awk '$1=="root"' >roots-procs.txt) \
               >(awk '$1!="httpd"' >not-apache-procs.txt) \
               >(awk 'NR>1{print $1}' >pids-only.txt)

---------------------------------------------------------------------------------------------------------
                          File descriptor redirections
---------------------------------------------------------------------------------------------------------
&>          # redirect both 1 and 2
M>N         # M by def is 1(fd) N is file name
M>&N        # N is fd
2>&1        # redirect stderr to stdout
1>&2        #      stdout to stderr 
|&          # abbrv for 2>&1
n<&-        # close input fd n
0<&-,<&-    # close stdin
n>&-        # close output fd n
1>&-,>&-    # close stdout



[n]<>filename   #open file <filename> for r/w and assign n fd to it used to write at a specified place in a file
echo 1234567890 > File    # Write string to "File".
exec 3<> File             # Open "File" and assign fd 3 to it.
read -n 4 <&3             # Read only 4 characters.
echo -n . >&3             # Write a decimal point there.
exec 3>&-                 # Close fd 3.

https://catonmat.net/bash-one-liners-explained-part-three
https://catonmat.net/bash-one-liners-explained-part-one
---------------------------------------------------------------------------------------------------------
                                    Examples
---------------------------------------------------------------------------------------------------------
Reading file line by line
---------------------------------------------------------------------------------------------------------
while read -r line; do
    # do something with $line
done < file
Note, leading and traling spaces are tuncated by IFS, to avoid this, reset IFS before reding like
while IFS= read -r line; do
...

Another way, instead of redirecting you can pipe
cat file | while IFS= read -r line; do
    # do something with $line
done

read -r random_line < <(shuf file)

Send the output from multiple commands to a file
(command1; command2) >file


Open two shells. In shell 1 do this:
mkfifo fifo
exec < fifo
In shell 2 do this:
exec 3> fifo;
echo 'echo test' >&3


=========================================================================================================
                                   Functions
=========================================================================================================
To hide global varible withing scope of your function, declare this variable as local. this will prevent
you from changing global one. Notice, local variables are seen by children functions
local global_var

Sometimes u want to generate command in runtime, for instance by creating string of command and its arguments.
This command wount work as expected if it contains redirections and pipes since they will be passed as
arguements instead of doing redirection. To get expected behaviour we need to run it in subshell using
command `eval` 

To list sourced functions, use `declare -f`, which will provide name and definition of a function. If you
don`t want definiton, use `declare -F`

If you want to find where function was defined issue next
shopt -s extdebug 
declare -F <function>
shopt -u extdebug

---------------------------------------------------------------------------------------------------------
                             Function substitution
---------------------------------------------------------------------------------------------------------
There are cases when you want to save a result of your command or call it in place, as parameter to another
function. This can be done with '$(cmd)' or ' `cmd` '. This syntax will expand command, call it and return
results. Difference between two syntaxes is next,`` is older version of $(), but `` needs to be escaped
when nested , like `cmd | \`cmd \` `. And $() applies to POSIX conformant shells, and can be not supported
by some old shels, this form is more easy to use when nesting '$( echo $(echo 123))', but is less readable
'Random string then $(cmd) then string ...'. There are hoax that $() is more resource heavy and faster but
this requires profiling to be sure.

=========================================================================================================
                                      Eval
=========================================================================================================
Eval can be a security issue, and need to be used with care.  It has chance of executing dirty data
 input="Trying to hack you; date"
 cmd=(echo "User gave:" "$input")
 eval "$(echo "${cmd[@]}")" # will execute date
call will be parsed twise, on the first pass quoting is removed and with quotes removed variables content
gets executed

=========================================================================================================
                                 Test operator
=========================================================================================================
[[ is improved version of [ for bash scripts

Hadles empty strings more intuitevly
[ -f "$file" ]    ==   [[ -f $file ]]

Lets use && || and <=> for strings (need to escaped with []), since it is just syntactic

Has regular expressions
if [[ $answer =~ ^y(es)?$ ]]

Pattern matching aka globbing for free?
Durring expansion of variable containing astrerisk [] will try expand it, [[]] wount

[[ $a == z* ]]   # True if $a starts with an "z" (pattern matching).
[[ $a == "z*" ]] # True if $a is equal to z* (literal matching).

[ $a == z* ]     # File globbing and word splitting take place.
[ "$a" == "z*" ] # True if $a is equal to z* (literal matching).

[ -s file ] # check if file is empty

=========================================================================================================
                                    History
=========================================================================================================
!! is the whole command line
!* is all the arguments of the command
!:1 is the first argument of the command
!:2 is the second argument of the command
!$ is the last argument of the command

=========================================================================================================
                                    Env vars
=========================================================================================================
$SECONDS time script is running
$OLDPWD    :path for cd -
$SHLVL     :sh call depth

=========================================================================================================
                                  Login Shell
=========================================================================================================
Login shell => first process executed under user ID when you log in for an interactive session. Login
shell has conventions like: $0 is prepended with '-'; files that usually have environmet variables are
read /erc/profile, ~/.prfile, ~/.bash_profile ...
When you log in on a text console, or through SSH you get an interactive login shell. Interactive means
that you can type into it and shell will execute your requests.
When you start a terminal in existing session, you get non-login interactive shell. This one usuallu avoid
environment configuration files and reads user files like bashrc. 
When you execute scripts they get a non-login, non-interactive shell which may involve reading some
start up files (bash runs $BASH_ENV)
More detailed information can be found in man bash

Check if shell is login-shell with one of the following:
shopt login_shell       # on
echo $SHLVL             # 2+
echo $-

List of valid login shells is saved to /etc/shells, if you try to login from something like /usr/shell
and it is not present within /etc/shells, login attempt will be rejected with PAM.
? it is updated with chsh

https://unix.stackexchange.com/questions/38175/difference-between-login-shell-and-non-login-shell
---------------------------------------------------------------------------------------------------------
                                    Profile
---------------------------------------------------------------------------------------------------------
On startup system will read .profile containing all globall setting of login shell. After that, whenever
user runs bash, files bash_aliases or bash_profile is issued, and if none of them found, local .profile
is issued. If current shell is login, then $0 will return executable name prefixed with '-', like
'-bash'

https://serverfault.com/questions/261802/what-are-the-functional-differences-between-profile-bash-profile-and-bashrc
---------------------------------------------------------------------------------------------------------
                                      Skel
---------------------------------------------------------------------------------------------------------
When user is created using adduser, or with useradd -s, skeleton from /etc/skel will be copied to its
$HOME. Another skel directory can be defined using -k <path>. To avoid flags like this, there is a
configuration file under /etc/default/useradd or /etc/adduser.conf.
Skell is usefull when you want to create user with predefined/default setup

https://askubuntu.com/questions/83532/how-do-i-set-up-new-users-with-skel
https://askubuntu.com/questions/971836/at-what-point-is-the-bashrc-file-created
=========================================================================================================
                                 Autocompletion
=========================================================================================================
Avoid using --colors=always for commands like grep since this may break already installed scripts wich
relay on output being clear ascii, like git autocompletion scripts
compspec (completion specification) is a list of registered completions, it can be found with `complete -p`
compspec entry looks like `complete <options> command_to_be_completed` 

Bashautocompletion may be present as result of several things. Some one may have written a function to
generate autocompletion result list and registered using complete -F _comlete_func_ <cmd>, and when
yoy press <tab> after <cmd>, function is trigered and list is generated. For some, bush may trigger <cmd>
will --help and parse the output. This case is registered with `complete -F _longopt ls`. 
Most of bash completions are saved withing apt packaged bash-completion and is installed to
/usr/share/bash-completion/ or /etc/bash_completion
Note, last defined completion for the same <cmd> overrides previous
? but what if cmd is specified in different forms like short path / full path / link to cmd / alias

Completion scripts have a convention, functions collecting reply have one `_` around name and helpers
have two

complete 
         -p <cmd>              # print all available completions for <cmd>, for all if no <arg> specified
         -r <cmd>              # -p but remove instead of print
         -E                    # empty line complition handler (when tab on empty line)
         -D                    # default handler, usually used as -D -F _complition_loader_
         -P/-S "<char>"        # prefix/suffix added to completed word
                               # complete -j -P '"%' -S '"' jobs
         -C <cmd>              # command is executed and result is used as complition
                               # command will have COMP_LINE and COMP_POINT env variables filled
                               # they will be duplicated into $1 and $2 as well ?
         -F <function>         # function is executed and result is used as complition
         -G <filter>           # filename expansion pattern 
         -X <filter>           # applied to completons generated by preceding options, each entry matching
                               # filter is removed from output, can be used with leading !, then all
                               # matching filter are displayed
                               # looks like only one is allowed, and is evaluated as last option
                               # -X '.[^./]*' to exclude ./ and ../ directories
         -W "<word_list>"      # completion with word list
         -A                    # Actions
            directory          # complete with list of directories in your `pwd`
            command            # list of command names -c , like for `which` 
            bindings           # Readline key binding names?
            disabled/enabled   # bash disabled/enabled bultins ?
            variable           # shell variables -v ?
            export             # exported variables -e
            hostname           # taken from $HOSTFILE (looks to be windows related stuff)
            setopt             # valid options for set -o
            shopt              # valid options for shopt
            signal             # signal names
            file (-f) alias (-a) arrayvar bultin (-b) directory (-d) function group (-g)
            helptopic(??) job (-j) keyword (-k) service (-s) user (-u) running/stopped 
         -o                    # comspec?
            bashdefault        # perform default bash completion if no complition was generated
            default            # ?
            dirnames           # same as -A direcotry?
            filenames
            nospace            # don`t apped space to completed words
            plusdirs           # ?

export FIGNORE='.tmp:.o'       # list of files to ignore for -f/-d

Note that you can chain completion options, they will be called one after another if previous had no
result
omplete -F _count_files -o dirnames script.sh

https://gerardnico.com/lang/bash/edition/complete

---------------------------------------------------------------------------------------------------------
                               Static completion
---------------------------------------------------------------------------------------------------------
In case u know all posible arguments to your program, u can define a word list of them and make it pop up
for autocompletion
complete -W "possible_arg1 possible_arg2 ..." cmd
Notice, options will be sorted unless `-o nosort` is used


---------------------------------------------------------------------------------------------------------
                               Dynamic completion
---------------------------------------------------------------------------------------------------------
To dynamically provide complition variants, we need to use functions and bash special variables
To register function, use
complete -F __func_name__ cmd

Registered function is responsible for filling available completions into array COMPREPLY, this will be
displayed on tab and will be filled again on completion request. To filter COMPREPLY by things that user
has already entered we can use
COMP_WORDS   ## array of all words fylly typed?, zero indexed
COMP_CWORD   ## index in COMP_WORDS, at the moment of completion request
COMP_LINE    ## current command line (full)
COMP_POINT   ## character number on which completion was requested?
COMP_KEY     ## ?
COMP_TYPE    ## ?
COMPREPLY    ## array of result filled with autocomlition, bash will copy this to cli when there is only
             ## one entry left
$1           ## program name
$2           ## word being completed (if you have entered part of word, empty otherwise)
$3           ## word before $2

To list possible completions as with complete actions, we can use `compgen` with `complete` flags, this
way we can filter COMPREPLY more easy. Compgen will out possible completiong to stdout, which we can save
to COMPREPLY array.

_dothis_completions()
{
  # Note, -- ${COMP_WORDS[COMP_CWORD]} lets you write couple of letters and then complete word
  # Note, complete is used to registrate autocompletion and compgen to generate entries
  COMPREPLY=($(compgen -W "ent1 ent2 $(ls) <or any another way to generate entries>" -- "${COMP_WORDS[COMP_CWORD]}"))
  echo "COMP_WORDS : ${COMP_WORDS}"
  echo "COMP_CWORD : ${COMP_CWORD}"
  echo "COMP_WORDS[COMP_CWORD] : ${COMP_WORDS[COMP_CWORD]}"
  echo "COMP_LINE : ${COMP_LINE}"
  echo "COMP_POINT : ${COMP_POINT}"
  echo "COMP_KEY : ${COMP_KEY}"
  echo "COMP_TYPE : ${COMP_TYPE}"
  echo "args : $@"
  echo "reply : ${COMPREPLY}"
}
complete -F _dothis_completions dothis


https://blog.bouzekri.net/2017-01-28-custom-bash-autocomplete-script.html
---------------------------------------------------------------------------------------------------------
                                    Readline
---------------------------------------------------------------------------------------------------------
There is also completion-like functionality that can be registered with `bind -x 'key:shell_func'`
READLINE_LINE => readline buffer contents within function used for bind -x
READLINE_POINT => current line position

Configuration file path is taken from $INPUTRC, if it is not defined, ~/.inputrc is the default. It is
source by program using readline library e.g. bash. You can reload it with 'C-x C-r' combination, which
comes with default configuration. Or you can just issue `bind -f <file>`

"\e[1;5C": forward-word
bind 'RETURN:"\e[Hreadline \C-g\n"'
bind -x '"\201":"echo 123"'
bind '"\C-g":"\201"'



TODO
man readline
https://unix.stackexchange.com/questions/290209/how-do-i-properly-source-inputrc
https://www.topbug.net/blog/2017/07/31/inputrc-for-humans/
---------------------------------------------------------------------------------------------------------
Execution order
---------------------------------------------------------------------------------------------------------
Command name is identified, if there is compspec it is used.
If there was no command (tab pressed on empty line), any compspec defined with -E is used.
If command specified is full path, look for comspec to fullpath, if not found, to command name following
final slash. If no result, apply any comspec with -D. If no result, try to expand as alias and lookup
compspec for it.
If no compspec found, use defaul bash completion behaviour, like environment completion after $
---------------------------------------------------------------------------------------------------------
Attempt to perform completion on the text before point. The actual completion performed is
application-specific. (Not sure what does application specific mean in this context. In my understanding,
this means when you create some compspec which is specific to this application and register it with complete,
but this page is referenced as default bash completion behaviour. Probably this means what I expect, since
description after this is really bash default and cant be found within complete -p). Bash attempts completion
treating the text as a variable (if the text begins with ‘$’), username (if the text begins with ‘~’),
hostname (if the text begins with ‘@’), or command (including aliases and functions) in turn. If none of
these produces a match, filename completion is attempted.
... # TODO, maybe finsh till end

In general case, you can simply print out registered compspec and look in which orded are flags. Bash will
sort them in order they are applied (except options like result quoting). 
https://www.gnu.org/software/bash/manual/html_node/Programmable-Completion.html
https://www.gnu.org/software/bash/manual/html_node/Commands-For-Completion.html#Commands-For-Completion
---------------------------------------------------------------------------------------------------------




https://www.gnu.org/software/bash/manual/html_node/Programmable-Completion.html
---------------------------------------------------------------------------------------------------------
https://stackoverflow.com/questions/14513571/how-to-enable-default-file-completion-in-bash

http://www.lbreyer.com/bash-smart-parsing-howto.html
https://gerardnico.com/lang/bash/edition/complete
https://spin.atomicobject.com/2016/02/14/bash-programmable-completion/
https://www.gnu.org/software/bash/manual/html_node/Programmable-Completion.html
https://iridakos.com/tutorials/2018/03/01/bash-programmable-completion-tutorial.html
https://unix.stackexchange.com/questions/12356/how-does-tab-auto-complete-find-options-to-complete
=========================================================================================================
                                    Getopts
=========================================================================================================
Usual way to use =>
while getopts "filter" <varname> <args>; do
  case $opt in
    ... options without dash ) ...
    \?) Invalid option ...
    :) Optin without reqired argument
  esac
done

This builtin utility initializes couple of variables
VARNAME ## shell variable used for option reporting
OPTARG  ## argument after option
OPTIND  ## index of next argument to be processed, can be used for shifting but then u are required to
        ##reset it to 1
OPTERR  ## whether bash should display errors or should it leave problem for you

Filter is a string of continiour or separated by ':' character .If filter is started with ':', then
verbose error reporting is disabled and next cases
invalid option => VARNAME = ? and OPTARG = option character
missing argument => VARNAME = : and OPTARG = option
If option requires an argument, put : after its name

---------------------------------------------------------------------------------------------------------
                                Double-dash '--'
---------------------------------------------------------------------------------------------------------
A double dash (--) is used in bash built-in commands to signify the end of command options, after which
only positional parameters are accepted. 

compgen -W "foo bar baz" -- b
grep -- -v <file>               # -v is treated not like flag but like a argument to search within file

https://unix.stackexchange.com/questions/11376/what-does-double-dash-mean-also-known-as-bare-double-dash
---------------------------------------------------------------------------------------------------------

=========================================================================================================
                                     Prompt
=========================================================================================================
PS1 => primary prompt
PS2 => secondaty promt , dispalyed for multiline commands
PS3 => prompt for bash select
PS4 => used to display debug log level?

=========================================================================================================
                           Run script without parent
=========================================================================================================
If u want to leave command active when terminal is closed, u can =>
- Make it immune to hangups / make it child of `init`
  nohup <cmd> &
- Disown the process
  <cmd> &
  disown
=========================================================================================================
                             Change starting shell
=========================================================================================================
To change starting shell for user, issue one of following commands are relogin
sudo usermod -s <shell> <username>
chsh -s <shell> <username>
=========================================================================================================
                                sudo
=========================================================================================================
To allow user issue commands under sudo, you should add him to sudo/admin group withing /etc/group
Privileges for sudo user are configured within /etc/sudoers
user host=(users:groups) commands
%sudo ALL=(ALL:ALL) ALL    => sudo user/group can run all commands on all hosts, all users and groups
Your local host is always saved within /etc/hosts and removing it may break sudo
For instance you can define user that can perform commands with sudo rights without entring password (but
with sudo prefix)
<user> ALL=(ALL:ALL) vim,cd,some_alias,some_script...
% before user, specifies user group.

If you want to execute command from another user on your system without relogin, issue
sudo -u <user> <cmd>

https://www.unixtutorial.org/how-to-use-visudo
---------------------------------------------------------------------------------------------------------
            Changing /etc/sudoers
---------------------------------------------------------------------------------------------------------
Since /etc/sudoers is important file, we should be carefull not to corrupt it. Imagine case when multiple
users would try to modify it at the same time, therefore making it unusable. For this reason, there is
utility named visudo, which locks file so it cannot be edited by anyone else simultaneously, as well as
some aditional features like syntax check. User trying to edit file at the same time will receive an error
offering to edit file later, or changes won`t be saved  It is really a wraper for vi ,nano or $EDITOR.


https://www.unixtutorial.org/how-to-use-visudo
---------------------------------------------------------------------------------------------------------
                                su vs sudo
---------------------------------------------------------------------------------------------------------
su stands for switch user and lets you run command as some other user under their user ID. If you issue
su without arguments, it will try to login root shell.
sudo stands for super user do, it allows you to run a command as root from your current user.

https://www.rootusers.com/the-difference-between-su-and-sudo-commands-in-linux/
---------------------------------------------------------------------------------------------------------
                            Proper way of running sudo 
---------------------------------------------------------------------------------------------------------
sudo resets most of environment variables, such as PATH, for security reason, therefore when you try to
execute some script or alias commnad will be unknown. To solve this issue you have next options:
sudo env "PATH=$PATH" <cmd>
sudo $(<cmd>)
sudo -i <cmd>                   # runs login shell, therefore most file like .profile are sourced
     -E                         # save user environment

To make bash expand aliases under sudo, do next
alias sudo="sudo " # this will make expand argument after sudo
https://unix.stackexchange.com/questions/139231/keep-aliases-when-i-use-sudo-bash
---------------------------------------------------------------------------------------------------------
=========================================================================================================
                              Man vs Info vs Help
=========================================================================================================
Man is unix traditional way of distributing documentation about programs. It was a collection of manual
pages "sections" correspond to sections in the full UNIX manual, but now there is too much of them to bind
them all into a single book. As well it has support for printing which is rarely done now.
Info is more powerfull replace of man, which supports hypelinkig and simpler markup compered to the troff.
GNU advocates against the use of man pages
In the end, the form in which you get documentation depends on the internal policies of the project that
provided the software in the first place -- there is no globally accepted standard.

Help in a bash builtin which uses internal structures to store and retrive information abouy bash commands

https://askubuntu.com/questions/9325/what-is-the-difference-between-man-and-info-documentation
=========================================================================================================
                       Locate vs Find vs Which vs Whereis
=========================================================================================================
1. Find is highly configurable filesystem scaner, if you have big filesystem and dont use some flags
to reduce the depth and breadth of your search it will take a lot of time
2. Locate uses a previously built, once a day by cron, database, if database is not updated then locate will
not show expected output. To sync the database issue `updatedb`
3, Which finds the binary of executable if it is your PATH
4. Whereis finds binary, sources and man pages for a program

https://askubuntu.com/questions/799776/what-is-the-difference-between-locate-whereis-which
=========================================================================================================
                                    Shebang
=========================================================================================================
On UNIX-like systems, you should alwasy start script with a shebang line. The system call execve (which
is responsible for starting programs) relies on an executable having either an executable header or a
shebang line. File passed to execve is either an executable object file, or a file of data for an interpreter.
An interpreter file begins with a line of the form: 
       #! interpreter [arg]
When you try to execute such file, execve will find '#!' symbol and path after it. Then file will be passed
again to that executable. Since usually it is some variant of shell/perl/python, shebang line will be skipped
since it is commented out with '#'.
You cannot always depend on the location of a non-standart program like bash. So it is generally a good
idea to use env, like '#!/usr/bin/env bash', but you cannot be sure that env is under /usr/bin

Shebang is considered to be human readable form of magic number in the executable file, the magic byte
string being 0x23 0x21 (ascii '#!') which will be used by exec functions to determine if it is a script
or executable. If file will be sourced within another script, then it is not mandatory to leave shebang
there.

https://stackoverflow.com/questions/25165808/should-i-use-a-shebang-with-bash-scripts
https://www.shellscript.sh/tips/shebang/
=========================================================================================================
                    Special parameters (internal variables)
=========================================================================================================
$$              # current shell pid (even if called from subshell)
$BASHPID        # current [sub]shell pid
$!              # last backgound process pid


man bash, special parameters for reference
=========================================================================================================
#           Not filtered
while read LINE;do  
  echo $LINE  
done <$1
value=$(<config.txt)

if [ -z "$1" ]
  then
    echo "No argument supplied"
fi

if [ $# -eq 0 ]
  then
    echo "No arguments supplied"
fi



\cp     |Overriding alias
command cp  |


The read command by default reads whole lines. So the solution is probably to read the whole line and then split it on whitespace with e.g. for:
while read line; do
    for word in $line; do
        echo "word = '$word'"
    done
done

read -a words
echo "${words[@]}"
for word in $(<inputfile)
#########################################################################################################
See if variable is set
[[ "$(sh -c 'echo ${qwe}')" ]] 
#########################################################################################################
Use trap cmd DEBUG to execute ur logic before cmd
Use PROMPT_COMMAND to execute ur logic after cmd
If u want to parse entered cmd , look at `bind` ->
http://www.lbreyer.com/bash-smart-parsing-howto.html

bind -x '"\C-e":hook'
hook(){ echo "hook: $READLINE_LINE";  }
bind 'RETURN: "\C-o\n"'

History utility `fc`
https://shapeshed.com/unix-fc/

=========================================================================================================
                                      Rare
=========================================================================================================
typeset     # list user functions with definitions
typeset -F  # list name only 

Subcommands like in git
https://stackoverflow.com/questions/37257551/defining-subcommands-that-take-arguments-in-bash
Timeout workaround for shells without timeout
http://mywiki.wooledge.org/BashFAQ/068
