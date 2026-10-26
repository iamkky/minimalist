# Minimal Unix Utilities

*A small collection of standalone replacements for GNU commands, built for fun, learning, and a minimalist Linux system.*

## How it started

While working on a new release of my custom minimalist Linux distribution, [**AbdLFS**](https://www.github.com/iamkky/abdlfs), based on [Linux From Scratch (LFS)](https://www.linuxfromscratch.org/), I was looking for things I could remove, simplify, or replace to make the system smaller.

That's when I noticed something curious: GNU Coreutils' `true` and `false` executables were each larger than 30 KB.

**30 KB for `true`? Really?**

After all, shouldn't `true` be little more than `return 0` or `exit(0)`, and `false` just `return 1` or `exit(1)`?

Of course, I know it sounds a little anachronistic to worry about 30 KB in an era when ordinary home computers have gigabytes of RAM and terabytes of storage.

But AbdLFS is a personal project. Its main goals are to **learn, experiment, and have fun**. It also serves as a foundation for some of my embedded projects. And I've always had a particular appreciation for minimalist solutions and the history and philosophy of Unix.

So I decided to investigate.

## Why is `true` so big?

GNU Coreutils is a mature and portable project. Its programs include common code for initialization, error handling, localization, command-line processing, and compatibility across different systems.

While this makes sense for a general-purpose distribution, it also adds overhead to the simplest utilities.

As a result, small programs can end up much larger than the code needed to perform its actual task.

I became curious about how other projects approached the same problem.

## Looking at the alternatives

I started exploring implementations from [BusyBox](https://busybox.net/), [Toybox](https://landley.net/toybox/), FreeBSD, and other Unix-like systems.

I found many much simpler implementations.

BusyBox and Toybox are excellent projects, and I really appreciate their approach. But neither was quite what I wanted.

**My goal isn't to replace GNU userspace or create a BusyBox-style distribution.**

I want to keep AbdLFS as a compact, conventional GNU/Linux system based on LFS.

Still, the idea of replacing `true` and `false` was too tempting to ignore.

So I wrote standalone replacements. And then I thought: why stop there?

## One thing led to another

After replacing `true` and `false`, I thought: why not replace `cksum` too? Besides the cksum calculation itself, it's a relatively simple utility with few options, and somewhat legacy these days, considering MD5, SHA, and other hashing algorithms.

I took Toybox's implementation and adapted it into a standalone program.

From there, a quick look through GNU Coreutils revealed several other commands that were surprisingly simple to reimplement, without any major challenges.

Many were little more than wrappers around standard C library functions or Linux system calls, in the most traditional and pure Unix style.

And as a Unix fanboy, I simply couldn't resist.

With AI helping, it became a fun exercise to find these little utilities, investigate they, and write simpler, compatible replacements without unnecessary bloat.

And frankly, having AI write the man pages made the whole thing even more enjoyable. It meant I could focus on learning how things work, making decisions, and having fun, while still ending up with something reasonably robust, and well documented fast.

## The collection

Each utility is written in C, with a corresponding manual page.

`basename`, `cksum`, `dirname`, `false`, `hostid`, `link`, `logname`, `printenv`, `sync`, `true`, `tty`, `unlink`, `whoami`

The implementation tries to replicate GNU Coreutils options, besides `--version` and `--authors`.

## Building and installing

The project includes a Makefile to compile and install the utilities.

Build and install everything:

```sh
make
make install
```

The manual pages are installed alongside the utilities.


## Feel free to use it

If you find any of these utilities useful for your own minimalist Linux installation, embedded system, LFS build, or just as examples of traditional Unix programming, **feel free to use them!**

Check the source files for applicable licensing terms, particularly code adapted from other projects.

Suggestions, corrections, and compatibility improvements are welcome.

This is a personal learning project, and having fun with it is very much part of the idea.

