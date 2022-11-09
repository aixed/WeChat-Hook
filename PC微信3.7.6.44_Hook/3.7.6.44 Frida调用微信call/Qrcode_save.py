from __future__ import print_function
import frida
import sys


# 当二维码刷新的时候 会调用保存二维码图片  所以你要自己扫描

def on_message(message, data):
    print(message)


def main(target_process):
    session = frida.attach(target_process)
    script = session.create_script("""var wechatWinAddress=Process.findModuleByName('WeChatWin.dll');
var img_renovate = wechatWinAddress.base.add('0x4C1EB0'); //二维码刷新点hook
var login_add = wechatWinAddress.base.add('0x25715A8');
Interceptor.attach(img_renovate, {
    onEnter(args) {
        var ecx = this.context.ecx;//指针指向一个保存图片的内存地址
        var size = Memory.readInt(ecx.add("4"));//二维码图片大小
        console.log("图片大小为："+size);
        var img_address = Memory.readInt(ecx);
        console.log("图片地址为："+img_address);
        var file_img=new File("G:/Qrcode_save/Qrcode.png", "wb");
        file_img.write(ptr(img_address).readByteArray(size));//这里由于是要传入一个本地指针  读取大概是 ecx寄存器 -> 图片指针->图片实际地址
        file_img.flush();
        file_img.close();
        console.log("二维码图片保存完成");
        
        // 以下是以字符串生成的登入  届时可以用第三方API将字符串形式生成二维码 比如彩色这种
        var login_text_add=Memory.readPointer(login_add);
        console.log("login_text_add"+login_text_add);
        
        var text = Memory.readCString(login_text_add);
        console.log("text"+text);
        console.log("http://weixin.qq.com/x/"+text);
        send({"http://weixin.qq.com/x/"+text});  
        
        
    }
});
""")
    script.on('message', on_message)
    script.load()
    print("[!] Ctrl+D on UNIX, Ctrl+Z on Windows/cmd.exe to detach from instrumented program.\n\n")
    sys.stdin.read()
    session.detach()


if __name__ == '__main__':
    main('wechat.exe')
