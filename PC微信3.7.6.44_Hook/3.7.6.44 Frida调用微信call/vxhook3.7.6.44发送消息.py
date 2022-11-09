from __future__ import print_function
import frida
import sys
import time


def on_message(message, data):
    print(message, data)


def main(target_process):
    session = frida.attach(target_process)
    script = session.create_script(open('WeChatSendTextMsg.js', encoding='utf-8').read())
    script.on('message', on_message)
    script.load()
    print("[!] Ctrl+D on UNIX, Ctrl+Z on Windows/cmd.exe to detach from instrumented program.\n\n")
    sys.stdin.read()
    session.detach()


if __name__ == '__main__':
    main('wechat.exe')
