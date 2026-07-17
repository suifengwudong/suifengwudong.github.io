---
layout: ../../layouts/BlogLayout.astro
title: "两个加速器，一副骨架：从 bohegamex 到雷神到 NN 的技术栈追溯"
pubDate: "2026-07-18"
author: "Suifeng Wudong"
tags: ["逆向", "游戏加速器", "NN", "雷神", "Electron", "Windows"]
description: "一台电脑上两个游戏加速器——雷神加速器和薄荷加速器（bohegamex）。不同品牌、不同公司、不同版本号，但底层共享从内核驱动到 CDN 的完整技术栈。本文追溯这套技术栈的源头：NN。"
---

# 两个加速器，一副骨架

## 一、 引子

我的电脑上安装了两款游戏加速器。

一款是**雷神加速器**（Leigod），完整安装在 `E:\LeiGod_Acc`，669 MB，正常运行。

另一款是**薄荷加速器**（bohegamex），只留下 227 MB 的 Electron 缓存残留在 `%APPDATA%`，本体早已被卸载。

它们是两款完全不同的软件——不同的品牌、不同的公司、不同的版本号、不同的安装路径。在任何一个用户眼里，"这是两个不同的产品"。

但当我开始深入挖掘它们的技术细节时，却发现了一个有趣的事实：**这两款加速器共享着从 Windows 内核驱动到 CDN 节点的完整技术栈，而连接它们的桥梁，是一个叫 NN 的技术平台。**

## 二、 表层差异

先看看它们表面的不同：

| 对比项 | 雷神加速器 | 薄荷加速器 (bohegamex) |
|--------|-----------|----------------------|
| 版本号 | 11.3.0.7 | 4.8.3.6 |
| 公司名 | 雷神（武汉）网络技术有限公司 | 武汉薄荷科技有限公司 |
| 官方网站 | leigod.com | jiasu.bohe.com |
| 安装目录 | E:\LeiGod_Acc\LeiGod_Acc | D:\BoheApps\Platform\ |
| ICP 备案 | 鄂ICP备19027593号-2 | 鄂ICP备18023477号-1 |
| 安装状态 | 正常使用 | 已卸载，残留 227 MB 缓存 |

看起来毫无关联。但当我们把视线从"产品"层面移开，聚焦到"技术"层面时，情况就完全不同了。

## 三、 NN——横跨两者的技术平台

### 3.1 什么是 NN？

NN 不是第三方供应商，而是雷神科技内部自研的技术品牌。

根据 nn.com 官网的描述：

> **NN（NN.com）是雷神开发的AI游戏赛事语音社区，具备AI全球专线网络加速能力**

雷神加速器官网也明确指出，"NN" 与 Windows、macOS、手机端、主机端、路由器并列为其六端通用的平台之一。

**NN 首先是一个技术平台，其次才是一个面向用户的产品。** 它既通过 Leigod.com 以品牌加速器的形态面向终端用户，也将其引擎打包、通过渠道合作伙伴分发。

### 3.2 引擎核心：从用户态到内核态

在雷神加速器的安装目录 `leishenSdk/` 下，存在一整套以 "nn" 命名的网络引擎组件：

**内核态驱动（Kernel Drivers）**

| 驱动 | 类型 | 作用 |
|------|------|------|
| `nnndisfilter.sys` | NDIS 过滤驱动 | 内核级网络包拦截与修改 |
| `NNDivert64.sys` | WinDivert | 用户态网络包捕获/注入 |
| `nfwfp.sys` | WFP 驱动 | Windows 过滤平台**
| `nftdi.sys` | TDI 驱动 | 传输驱动接口 |
| `tap0901.sys` | TAP 虚拟网卡 | VPN 虚拟网卡 |

这些驱动分别针对 Win7、Win8、Win10 体系编译了多个版本，覆盖了完整的 Windows 版本生态。

**用户态组件（User-Mode DLLs）**

| 组件 | 版本 | 说明 |
|------|------|------|
| `nne.dll` | 1.1.8.3 | NDIS 过滤加速引擎——NN 引擎核心 |
| `nnlsp.dll` | 1.0.1.1 | NN LSP 协议代理 |
| `nnlsp64.dll` | 1.0.1.1 | NN LSP 协议代理（64位） |
| `tun2socks.dll` | — | VPN 隧道转换 |
| `VpnProxy.dll` | — | VPN 代理 |

`nne.dll` 的文件属性显示其所属公司为**武汉雷神科技有限公司**（雷神集团内部负责引擎开发的实体），产品名为 `nne`，描述为 "ndisfilter acc engin"。

**这就是 NN 引擎的核心——一套从内核 NDIS 驱动到用户态协议代理的完整网络加速技术栈。**

### 3.3 网络基础设施

NN 引擎运行在一套统一的基础设施之上：

| 层级 | 组件 | 详情 |
|------|------|------|
| **CDN** | `dfs01.nn.com` | 图片/静态资源分发 |
| **CDN 供应商** | `kunluncan.com` | 58.205.221.x 节点集群 |
| **DNS** | `vip3.alidns.com` / `vip4.alidns.com` | 阿里云企业级 DNS |
| **云平台** | 腾讯云（北京） | 150.158.x.x C 段 |
| **主 API** | `api1.leigod.com` | 业务配置接口 |
| **安全 API** | `secapiv11.leigod.com` | 加签敏感接口 |

加速隧道实际流量走向（从日志提取）：

```
用户 → nnndisfilter.sys（本地拦截）
     → VPN 隧道（tun2socks / VpnProxy）
     → TCP/UDP 代理（香港出端口 14.17.69.215）
     → 游戏服务器
```

### 3.4 游戏加速通道

雷神加速器支持 **8288 款游戏**的加速（来自 `game_path.json` 数据库），涵盖 Steam、Epic、以及大量国产游戏。每次加速会话使用统一的 `game_id` 标识，例如加速 PUBG 时使用 `game_id: 109`。

薄荷加速器（bohegamex）的缓存中同样发现了游戏加速通道列表，包含了 `steamgamehub1~210`、`gamehub`、`steamblue`、`yingyun` 等通道命名——**这些通道的结构与雷神加速器的游戏数据库一致，使用的是同一套加速节点体系。**

## 四、 两个加速器，一个 NN

### 4.1 雷神加速器 = NN 自营品牌

雷神加速器由雷神（武汉）网络技术有限公司自营。以下证据证明它与 NN 的直接隶属关系：

**证据一：公司主体一致性**
雷神加速器主程序（`leigod.exe`）文件属性显示公司为"雷神（武汉）网络技术有限公司"，其 ICP 备案号为鄂ICP备19027593号-2。NN 引擎核心（`nne.dll`）的所属公司为"武汉雷神科技有限公司"——这两家公司分别是雷神集团的运营实体和研发实体，而 NN 本身就是雷神旗下的技术平台品牌。

**证据二：渠道标识**
雷神加速器 HTTP 请求中携带 `src_channel=guanwang_nn`，明确标识其分发渠道为"NN 官网"。API 配置数据中还包含了 `nn_url` 字段，广告内容直接跳转至 `web.nn.com/community/...?source=leigod`——Leigod 客户端直接为 NN 社区导流。

**证据三：六端通用中的 NN**
雷神加速器官网明确将 NN 列为"六端通用"的一环，与 Windows、macOS、手机端、主机端、路由器并列。这明确将 NN 定位为雷神加速器的自属平台，而非第三方合作方。

**证据四：日志中的技术绑定**
HTTP 日志显示，雷神加速器启动后立即请求 `api1.leigod.com` 和 `secapiv11.leigod.com` 获取配置，所有请求通过 NN 引擎的 NDIS 过滤驱动进行加速转发。Socket 日志显示了完整的加速隧道数据：使用 `game_id: 109` 标识游戏（PUBG），通过香港节点（`14.17.69.215`，`note_outer_ip: HK`）建立 TCP/UDP 代理隧道，全程延迟约 40ms、丢包率 0%。

### 4.2 薄荷加速器 = NN 渠道合作

薄荷加速器并非雷神直接运营，而是通过渠道合作模式分发。以下证据支撑这一结论：

**证据一：独立公司主体**
ICP 备案显示 `bohe.com` 的运营主体为**武汉薄荷科技有限公司**（鄂ICP备18023477号-1），与雷神集团（鄂ICP备19027593号-2）是两家独立的公司法人。两家公司的注册信息无股权关联。

**证据二：百度渠道分发**
从 bohegamex 缓存中提取到的 `channelPackageName=baidusteam15wdbohe` 明确标记了其分发来源：百度（baidu）+ Steam 游戏加速（steam）+ 渠道版本号（15wd）+ 品牌标识（bohe）。相比之下，雷神加速器的渠道标识为 `guanwang_nn`——两者分别走百度渠道和官网渠道，互不重叠。

**证据三：NN 基础设施复用**
薄荷加速器使用 `dfs01.nn.com` 作为 CDN（与雷神加速器共用同一 IP 集群 `58.205.221.x`），其 `appId` 直接命名为 `nnPcClient_swbTF87Q`，加速通道列表（`steamgamehub1~210`、`gamehub`、`steamblue`、`yingyun`）与 NN 引擎的游戏加速节点体系一致。这表明薄荷加速器并未自建基础设施，而是直接调用 NN 的加速能力。

**证据四：跨品牌推广**
缓存中 `showLeicgodMenu: true` 表明薄荷加速器内置了雷神加速器的推广入口——这在渠道合作模式中是常见的交叉推广策略。

两者版本号体系完全不同（11.x vs 4.x），说明薄荷加速器是基于 NN 引擎 SDK **独立开发**的客户端，而非简单的"换皮"。

### 4.3 商业关系

从 ICP 备案查询来看：

| 域名 | 公司 | 关系 |
|------|-----|------|
| leigod.com | 雷神（武汉）网络技术有限公司 | NN 技术研发 + 自营品牌 |
| bohe.com | 武汉薄荷科技有限公司 | 渠道合作伙伴 |
| nn.com | 雷神旗下平台 | 技术平台品牌 |

两者是独立的公司法人，薄荷加速器通过**百度渠道**（`baidusteam15wdbohe`）分发，以自有品牌面向用户。但底层的加速技术——从内核驱动到 CDN 到加速节点——全部来自 NN。

## 五、 完整图景

```
                  ┌─────────────────────────┐
                  │      NN 技术平台         │
                  │   （雷神科技研发运营）    │
                  │                         │
                  │   ┌─────────────────┐   │
                  │   │  内核驱动层      │   │
                  │   │  nnndisfilter.sys │   │
                  │   │  NNDivert64.sys  │   │
                  │   │  nfwfp.sys       │   │
                  │   └────────┬────────┘   │
                  │            │             │
                  │   ┌────────v────────┐   │
                  │   │  用户态引擎层    │   │
                  │   │  nne.dll 1.1.8.3│   │
                  │   │  nnlsp.dll      │   │
                  │   │  tun2socks.dll  │   │
                  │   └────────┬────────┘   │
                  │            │             │
                  │   ┌────────v────────┐   │
                  │   │  基础设施层      │   │
                  │   │  CDN: dfs01.nn  │   │
                  │   │  API: *.leigod  │   │
                  │   │  DNS: 阿里云    │   │
                  │   │  云平台: 腾讯云 │   │
                  │   └────────┬────────┘   │
                  │            │             │
                  │   ┌────────v────────┐   │
                  │   │  加速通道层      │   │
                  │   │  8288 款游戏    │   │
                  │   │  HK/全球节点    │   │
                  │   │  TCP/UDP 代理   │   │
                  │   └─────────────────┘   │
                  └─────────────────────────┘
                            │
            ┌───────────────┴───────────────┐
            │                               │
            ▼                               ▼
   ┌──────────────────┐          ┌──────────────────┐
   │   雷神加速器       │          │  薄荷加速器       │
   │   (自营品牌)       │          │  (渠道合作)       │
   │                   │          │                  │
   │  leigod.com       │          │  jiasu.bohe.com  │
   │  雷神(武汉)网络    │          │  武汉薄荷科技     │
   │  v11.3.0.7        │          │  v4.8.3.6        │
   │  guanwang_nn 渠道  │          │  baidusteam15wdbohe│
   │  NN 引擎全集成     │          │  NN 引擎 SDK     │
   └──────────────────┘          └──────────────────┘

         两个品牌，一个 NN——"两个加速器，一副骨架"
```

## 六、 行业透视：加速器的白标模式

在中国游戏加速器市场，"自研引擎"是个很常见的营销话术。但实际上，构建一套从内核驱动到全球节点的完整加速基础设施，技术门槛和资金投入极高。这催生了一种**技术提供商 + 渠道分销商**的商业模式：

- 技术提供商（如雷神/NN）建设底层基础设施
- 渠道商（如薄荷）借用这套基础设施，以自己的品牌和流量渠道面对用户
- 用户购买的加速服务，底层其实是同一份资源

这种模式在行业内并不罕见——类似航空公司的里程转售给银行作为信用卡积分，底层的飞行服务是相同的，但用户面对的"积分品牌"各不相同。

## 七、 隐私与安全建议

作为这次调查的副产品，需要指出的是，游戏加速器（无论品牌）通常需要安装内核级网络驱动才能实现加速功能。这类驱动拥有对系统网络栈的完全访问权限，在安装时应确认来源可信。

另外，Electron 应用的缓存目录中可能残留敏感信息（Token、实名认证数据等），如下清理：

```powershell
# 清理 bohegamex 残留缓存
Remove-Item "$env:APPDATA\bohegamex" -Recurse -Force

# 检查 APPDATA 下其他可疑残留
Get-ChildItem "$env:APPDATA" -Directory | ForEach-Object {
  $size = (Get-ChildItem $_.FullName -Recurse -File -ErrorAction SilentlyContinue |
            Measure-Object -Property Length -Sum).Sum
  [PSCustomObject]@{ Folder = $_.Name; SizeMB = [math]::Round($size/1MB, 2) }
} | Sort-Object SizeMB -Descending
```

## 八、 参考资料

| # | 来源 | URL | 内容 |
|---|------|-----|------|
| [1] | 雷神加速器官网 | https://www.leigod.com | 公司信息、产品线、ICP 备案 |
| [2] | 雷神关于我们 | https://www.leigod.com/company/about | "六端通用"含 NN |
| [3] | NN 官方网站 | https://www.nn.com | "雷神开发的AI游戏赛事语音社区" |
| [4] | bohe.com ICP 备案 | https://icplishi.com/bohe.com/ | 武汉薄荷科技有限公司 |
| [5] | 薄荷加速器官网 | https://jiasu.bohe.com | bohegamex 前端品牌 |
| [6] | nne.dll 文件属性 | E:\LeiGod_Acc\...\leishenSdk\nne.dll | NDIS 加速引擎，武汉雷神科技 |
| [7] | Leigod HTTP 日志 | E:\LeiGod_Acc\...\llogs\http-2025-12-5.log | API 端点、隧道参数、src_channel |
| [8] | Leigod Socket 日志 | E:\LeiGod_Acc\...\llogs\socket-2025-12-5.log | 加速会话数据、game_id、HK 节点 |
| [9] | bohegamex LevelDB | %APPDATA%\bohegamex\Local Storage | 渠道信息、会员体系、用户数据 |
| [10] | Leigod 引擎驱动 | E:\LeiGod_Acc\...\leishenSdk\Driver\lwf\ | nnndisfilter.sys 等内核驱动 |
