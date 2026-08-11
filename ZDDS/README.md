# ZDDS 库目录

请将 ProtocolSimulator 项目中的 ZDDS 库文件拷贝到此处，目录结构如下：

```
TcpZddsProxy/ZDDS/
├── include/
│   └── zdds.h           (来自 ProtocolSimulator/src/ZDDS/include/zdds.h)
└── lib/
    ├── libZDDSd.so      (来自 ProtocolSimulator/src/ZDDS/lib/libZDDSd.so)
    ├── libZDDSd.so.1    (符号链接)
    ├── libZDDSd.so.1.2  (实际库文件)
    ├── libzmq.so        (来自 ProtocolSimulator/src/ZDDS/lib/libzmq.so)
    └── libzmq.so.5      (libzmq 实际库文件)
```

## 获取方式

从 https://github.com/jiangzhuo88/ProtocolSimulator 项目下载或克隆：

```bash
git clone https://github.com/jiangzhuo88/ProtocolSimulator.git
cp -r ProtocolSimulator/src/ZDDS/include TcpZddsProxy/ZDDS/
cp -r ProtocolSimulator/src/ZDDS/lib TcpZddsProxy/ZDDS/
```

注意：`libZDDSd.so` 中的 `d` 后缀代表 debug 版本，如果使用 release 版本，请调整 .pro 文件中的 `-lZDDSd` 为对应名称（如 `-lZDDS`）。
