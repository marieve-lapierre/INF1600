#!/usr/bin/env python3

import os
import sys
from babeltrace import TraceCollection, CTFScope
import pprint
import shutil

from os.path import join, dirname, abspath
import string
import subprocess
from optparse import OptionParser

orig_pwd = abspath(os.path.curdir)
top_srcdir = dirname(dirname(abspath(sys.argv[0])))
trace_home = os.environ.get("LTTNG_TRACE_HOME", os.path.join(orig_pwd, "traces"))

chunk = {
    "byte": 1,
    "word": 8,
    "page": 1 << 12,
    "huge": 1 << 20,
}

experiences = {
    "heap_nofill_huge": {
        "where": "heap",
        "block-size": "huge",
        "block-count": 150,
        "fill": False },
    "heap_fill_huge": {
        "where": "heap",
        "block-size": "huge",
        "block-count": 150,
        "fill": True },
}

gnuplot_template = \
"""set terminal pngcairo size 640,480 enhanced font 'Verdana,10'
set output 'pid_%(pid)d.png'
set title "Memory analysis %(name)s"
set xlabel "time (ns)"
set ylabel "size (byte)"
# set key outside
plot "pid_%(pid)d.data" using 1:2 with lines title "heap", "pid_%(pid)d.data" using 1:3 with lines title "stack", "pid_%(pid)d.data" using 1:4 with lines title "kmem"
"""

def load_traces(base):
    traces = TraceCollection()
    for root, dirs, files in os.walk(base):
        for f in files:
            if f == "metadata":
                ret = traces.add_trace(root, "ctf")
                if ret is None:
                    raise IOError("Error adding trace %s" % root)

    return traces

def process_trace(base, name, output):
    traces = load_traces(base)
    counts = {}
    state = {} # (pid,type,value)
    data = {} # (pid, file)
    start = None
    ctx = CTFScope.STREAM_EVENT_CONTEXT
    for event in traces.events:

        # prepare
        if not start:
            start = event.timestamp
        if not event.name in counts:
            counts[event.name] = 0
        counts[event.name] += 1

        pid = event.field_with_scope("vpid", ctx)
        if pid is None:
            pid = event.field_with_scope("pid", ctx)
            if pid is None:
                raise Exception("missing pid/vpid context")
        if not pid in state:
            state[pid] = {'heap': 0, 'stack': 0, 'tos': None, 'kmem': 0, 'ptrs': {}}
        stx = state[pid]

        # process event

        if event.name == 'lttng_ust_libc:malloc':
            sz = event['size']
            stx['heap'] += sz
            stx['ptrs'][event['ptr']] = sz
        elif event.name == 'lttng_ust_libc:calloc':
            sz = event['nmemb'] * event['size']
            stx['heap'] += sz
            stx['ptrs'][event['ptr']] = sz
        elif event.name == 'lttng_ust_libc:free':
            sz = stx['ptrs'].pop(event['ptr'], 0)
            stx['heap'] -= sz
        elif event.name == 'stack:entry':
            if stx['tos'] is None:
                stx['tos'] = event['rsp']
            stx['stack'] = abs(event['rsp'] - stx['tos'])
        elif event.name == 'kmem_mm_page_alloc':
            stx['kmem'] += 4096
        elif event.name == 'kmem_mm_page_free':
            stx['kmem'] -= 4096

        # output
        if stx['heap'] > 0 and stx['stack'] > 0 and stx['kmem'] > 0:
            if pid not in data:
                out = open(os.path.join(output, 'pid_%d.data' % pid), 'w')
                out.write("ts heap stack kmem\n")
                data[pid] = out

            ts = event.timestamp - start
            data[pid].write("%d %d %d %d\n" % (ts, stx['heap'], stx['stack'], stx['kmem']))

    # terminate
    old_dir = abspath(os.path.curdir)
    os.chdir(output)
    for pid, f in data.items():
        f.close()
        gnuplot_file = "pid_%d.gplot" % pid
        gnuplot = open(os.path.join(output, gnuplot_file), 'w')
        gnuplot.write(gnuplot_template % {'pid': pid, 'name': name})
        gnuplot.close()
        subprocess.call(["gnuplot", gnuplot_file])
    os.chdir(old_dir)

# let's fix max-mem to be always 1000 * chunk size
def build_drmem_cmd(options, name, exp):
    cmd =  [ "lttng-simple", "-c", "-u", "-k", "-o", trace_home ]
    cmd += [ "-e", "mm" ]
    cmd += [ "--enable-libc-wrapper" ]
    cmd += [ "--stateless", "--name", name, "--" ]
    cmd += [ join(options.drmem_dir, "drmem") ]
    cmd += [ "--where", exp.get("where", "heap") ]
    bs = exp.get("block-size", "page")
    cmd += [ "--block-size", bs ]
    bc = exp.get("block-count", 100)
    # allocate at least 1MB
    max_mem = bc * chunk[bs]
    if (max_mem < (1 << 20)):
        max_mem = 1 << 20

    cmd += [ "--max-mem", str(max_mem) ]
    cmd += [ "--delay", "5" ]
    fill = exp.get("fill", True)
    if (fill): cmd += [ "--fill" ]
    trim = exp.get("trim", None)
    if (trim):
        cmd += [ "--trim", str(trim) ]
    return cmd

def run_cmd(cmd):
    ret = subprocess.call(cmd)
    if ret != 0:
        raise RuntimeError("Command failed: " + string.join(cmd, " "))

def do_one(options, name, exp):
    output_dir = join(top_srcdir, "results", name)
    if os.path.exists(output_dir):
        shutil.rmtree(output_dir)
    os.makedirs(output_dir)

    # make trace
    cmd1 = build_drmem_cmd(options, name, exp)
    print(cmd1)
    run_cmd(cmd1)

    # analysis
    base = os.path.join(trace_home, name + "-k-u")
    process_trace(base, name, output_dir)


def which(name, flags=os.X_OK):
    result = []
    exts = filter(None, os.environ.get('PATHEXT', '').split(os.pathsep))
    path = os.environ.get('PATH', None)
    if path is None:
        return []
    for p in os.environ.get('PATH', '').split(os.pathsep):
        p = os.path.join(p, name)
        if os.access(p, flags):
            result.append(p)
        for e in exts:
            pext = p + e
            if os.access(pext, flags):
                result.append(pext)
    return result


usage = """usage: %prog [options] [experiment1, experiment2, ...]

Execute drmem experiments.
"""

if __name__=="__main__":
    drmem_dir = os.path.join(top_srcdir, "drmem")
    parser = OptionParser(usage=usage)
    parser.add_option("--dry-run", dest="dry_run", default=False, action="store_true", help="display commands and do not execute them")
    parser.add_option("--list", dest="list", default=False, action="store_true", help="display available experiments")
    parser.add_option("--drmem-dir", dest="drmem_dir", default=drmem_dir, help="drmem directory")
    
    (options, args) = parser.parse_args()
    if (options.list):
        print("available experiences:")
        for name, opts in experiences.items():
            print(name)
        sys.exit(0)

    print(options)
    print(args)
    # validate experiences
    for arg in args:
        if not arg in experiences : #experiences.has_key(arg):
            raise Exception("unknown experience %s" % (arg))
    
    # check for required tools
    ok = True
    exe_list = [ "lttng-simple", "lttng", "gnuplot" ]
    for exe in exe_list:
        res = which(exe)
        if len(res) == 0:
            ok = False
            print("Not found in path: " + exe)
    if not os.path.exists(options.drmem_dir):
        print("drmem not found")
        ok = False
    if not ok:
        print("Can't run analysis, verify the setup")
        sys.exit(1)

    # put the lttng-simple profile in the home directory
    profile_dir = os.path.join(os.environ.get("HOME"), ".workload-kit")
    if not os.path.exists(profile_dir):
        os.makedirs(profile_dir)
    shutil.copy(os.path.join(orig_pwd, "mm.list"),
                os.path.join(profile_dir, "mm.list"))

    try:
        if len(args) == 0:
            # run all experiences
            for name, opts in experiences.items():
                do_one(options, name, opts)
        else:
            for arg in args:
                do_one(options, arg, experiences[arg])
    except (KeyboardInterrupt, RuntimeError) as e:
        os.chdir(orig_pwd)
        print("destroy sessions...")
        run_cmd(["lttng", "destroy", "-a"])
        #print(str(e))
        
