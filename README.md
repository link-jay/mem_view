# mem_view
A simple memory view program in C.  
It's built on the Linux virtual memory system, relying on parsing /proc/[pid]/maps and reading /proc/[pid]/mem to achieve its functionality.

## Features
* [x] View heap/stack
* [x] Refresh when size change
* [x] Choose refresh time
* [x] Switch heap/stack view via command line
* [ ] Switch heap/stack view using hotkeys

## Args
`./mem_view.out [-s|-h|-t 200|--help] target`  
`-s`: View stack(default option).  
`-h`: View heap.  
`-t time`: Change refresh time(default 0.5s).  
`--help`: Print help message.  

## Arch
![Arch](./arch.svg)
