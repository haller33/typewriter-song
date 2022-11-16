# Typewriter-song

This is a [bpftrace]-fork program to run locally that turns your keyboard into a noisy typewriter. Keystrokes make a "click" and the enter key makes a "ting".

This works by tracing the Linux tty driver using kprobes. [Brendan Gregg](ttps://github.com/brendangregg) first wrote tracing-typewriter tools in [2005](https://github.com/brendangregg/bpf-typewriter).

## Dependencies

- [bpftrace]
- pipeweire for the `pw-play` command. You can edit the source to use a different audio player.

## Wy a "Fork" ?

because the project with bpftrace need root acess and my root user can't play sound on my pipewire 
configuration, so i created a client-server side, to inform a key press and play the sound on user space. 

also the project have use of a lot of resources because relay on OS process level of fork, to have to play 
the sound, this project relay on greenthreads for that and single-header files like [stb](https://github.com/nothings/stb)

remember that the amount of code used and created may declassify this a fork of project, and maybe another project by it self.

## Build

Just use [nobuild](https://github.com/tsoding/nobuild) build system, with some extensions for this project im [particular](https://github.com/haller33/typewriter-song/blob/master/include/nobuild_extended.h), 
for meta programming.

```
make first
./first 
./bin/typewriter
```

also can be run the daemon on another machine and play the typewriter sound on a stereo setup.

## Source

Just the code:

```
#!/usr/bin/bpftrace --unsafe
/* copy from https://github.com/brendangregg/bpf-typewriter */

kprobe:kbd_event
/arg1 == 1 && arg3 == 1/
{
        if (arg2 == 28) {
	
                 system("./bin/typewriter-song-sender ENT&");
        } else {
       
	         system("./bin/typewriter-song-sender &");
        }
}

```

the resource is also a systemd service, and TODO need to make more changes to compile eBPF programs from C source code.

## considerations

I would have traced kbd_rawcode(), but it appears inlined and unavailable to kprobes. The --unsafe option is necessary because this tool is launching commands via system().

[bpftrace]: https://github.com/iovisor/bpftrace
[2005]: http://www.brendangregg.com/specials.html#typewriter
