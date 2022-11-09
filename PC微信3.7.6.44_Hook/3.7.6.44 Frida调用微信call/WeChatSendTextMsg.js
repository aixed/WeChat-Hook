let ModAddress=Process.findModuleByName('WeChatWin.dll');
console.log('Mod Address:' + ModAddress.base);

let callAddress=ModAddress.base.add('0x5CD2E0')
console.log('Hook Address: ' + callAddress);

let msg_text="来自frida的消息"
let wxid="filehelper"

let msg_add=Memory.allocUtf16String(msg_text);
let wxid_add=Memory.allocUtf16String(wxid);

console.log('msg_add: ' + msg_add);
console.log('wxid_add: ' + wxid_add);
let buff= Memory.alloc(1024);
console.log('buff: ' + buff);
let struct_msg=Memory.alloc(1024);
console.log('struct_wxid: ' + struct_msg);
struct_msg.writePointer(msg_add);
console.log('写入消息内容成功！');
struct_msg.add(4).writeInt(msg_text.length*2);
console.log('写入消息内容长度1成功');
struct_msg.add(8).writeInt(msg_text.length*2);
console.log('写入消息内容长度2成功');



let struct_wxid=Memory.alloc(1024);
//console.log('写入wxid成功！');
struct_wxid.writePointer(wxid_add);
//console.log('写入wxid指针成功！');
struct_wxid.add(4).writeInt(wxid.length*2);
// console.log('写入wxid 1 指针成功！');
struct_wxid.add(8).writeInt(wxid.length*2);
//.log('写入wxid 2 指针成功！');

//Memory.allocUtf8String(str) 78B2D2E0  78b2d2e0

const fastCallback = Memory.alloc(Process.pageSize);
Memory.patchCode(fastCallback, 128, code => {
    const cw = new X86Writer(code, { pc: fastCallback });


    // console.log('struct_wxid: ' + struct_wxid);
    // console.log('buff: ' +buff.toUInt32())
    // console.log('struct_msg: ' +struct_msg.toUInt32())
    cw.putPushU32(buff.toUInt32()); //push 07960000h
    cw.putPushU32(0);//push 00000000h
    cw.putPushU32(1);//push 00000001h
    cw.putPushU32(0);//push 19BF0000h
    cw.putPushU32(struct_msg.toUInt32());//push 099E0000h
    cw.putMovRegU32("edx", struct_wxid.toUInt32());//mov edx, 19BD0000h
    cw.putMovRegU32("ecx", buff.toUInt32());//mov ecx, 07960000h
    cw.putCallAddress(callAddress);//mov ebx, 78B2D2E0h call ebx
    cw.putAddRegImm("esp", 20);
    cw.putRet();
    cw.flush();
    console.log('汇编执行结束');
    //push 07960000h
    // push 00000000h
    // push 00000001h
    // push 19BF0000h
    // push 099E0000h
    // mov edx, 19BD0000h
    // mov ecx, 07960000h
    // mov ebx, 78B2D2E0h
    // call ebx
    // add esp, 14h
    // ret
    const callMemFun = new NativeFunction(fastCallback, 'void', []);
    callMemFun();

})
