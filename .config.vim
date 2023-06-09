" this config is for project

" name     : the task name
" mode     : (new version add)the mode of task,quickfix,term,workflow,shell
"            workflow first exec task [begin],and if first exec is ok,exec task [end]
"            use workflow mode,first exec must be quickfix
"            workflow mode option is only with:begin,next,name,key
"            shell model only run in backgroud and show nothing which is always success
" command  : the command to run
" path     : path can be termtask#Term_get_dir() stand for git,expand("%:p:h") for current dir
"            expand("%") for current buffer all path,expand("%:t") for current buffer name
"            use . can connect str
" close    : term and quickfix after command status
"            0 noclose
"            1 close
"            2 if command is not ok,will stay open,or close,
"            3 do not open first,after finish,open it,
" type     : pos of task tab or vsplit or split
" key      : key can bound map to make task run
" script   : pre,end script is vimscript,use | to divide
"            can define function,and make end_script call func to do it(recommand way)
"            can use g:asyncrun_code==0 to judge task if success
let s:root=termtask#Term_get_dir()
let s:pwd=expand('%:p:h')
let g:Term_project_task=[
			\{
			\'name'       : 'build',
			\'command'    : 'g++ -O2 '.expand('%'),
			\'mode'       : 'quickfix',
			\'path'       : s:root,
			\'close'      : 2,
			\'type'       : 'split',
			\'key'        : '\1',
			\'pre_script' : '',
			\'end_script' : '',
			\},
			\{
			\'name'       : 'run',
			\'command'    : './a.out',
			\'mode'       : 'term',
			\'path'       : s:root,
			\'close'      : 0,
			\'type'       : 'vsplit',
			\'key'        : '\2',
			\},
			\{
			\'name'       : 'workflow',
			\'mode'       : 'workflow',
			\'begin'      : 'build',
			\'next'       : 'run',
			\'key'        : '\3',
			\},
			\{
			\'name'       : 'test',
			\'command'    : 'make;./main',
			\'mode'       : 'term',
			\'path'       : s:root."/test",
			\'close'      : 0,
			\'type'       : 'vsplit',
			\},
			\]

